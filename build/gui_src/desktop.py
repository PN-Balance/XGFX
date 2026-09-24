from __future__ import annotations

import base64
import io
import json
import os
import re
import subprocess
import sys
import threading
from pathlib import Path

from PIL import Image

# 开发源码位于 build/gui_src；打包后 PyInstaller 已包含核心模块。
if not getattr(sys, 'frozen', False):
    sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools' / 'xgfx_script'))
import xgfx_asset as core


def png_url(image):
    buffer = io.BytesIO()
    image.save(buffer, format='PNG')
    return 'data:image/png;base64,' + base64.b64encode(buffer.getvalue()).decode('ascii')


def render_asset(asset):
    c = asset.config
    if c.image_type == 'MIRROR':
        colors = [core.device_to_rgb(v, c.color_format) for v in asset.device_pixels]
    elif c.image_type == 'BITMAP':
        colors = [(round(v * 255 / ((1 << c.bpp)-1)),)*3 for v in asset.indices]
    else:
        colors = [core.device_to_rgb(asset.palette_values[i], c.color_format) for i in asset.indices]
    alpha = [255] * len(colors)
    if c.alpha_bpp:
        alpha = [round(v*255/((1 << c.alpha_bpp)-1)) for v in core.unpack_rows(asset.alpha_data, asset.width, asset.height, c.alpha_bpp)]
    image = Image.new('RGBA', (asset.width, asset.height))
    image.putdata([(*rgb, a) for rgb, a in zip(colors, alpha)])
    return image


class Api:
    def __init__(self, root=None):
        self._root = Path(root).resolve() if root else None
        self._window = None
        self._allow_close = False
        self._lock = threading.Lock()
        self._progress = {'running': False}
        self._prefs = self._load_prefs()
        self._recent: list[str] = list(self._prefs.get('recent', []))

    @staticmethod
    def _prefs_path() -> Path:
        base = Path(os.environ.get('LOCALAPPDATA') or Path.home())
        return base / 'XGFX' / 'preferences.json'

    def _load_prefs(self) -> dict:
        try:
            path = self._prefs_path()
            if path.is_file():
                return json.loads(path.read_text(encoding='utf-8-sig'))
        except (OSError, json.JSONDecodeError):
            pass
        return {'theme': 'dark', 'recent': [], 'view': 'folders',
                'sort': {'key': 'name', 'direction': 'asc'}}

    def _save_prefs(self):
        try:
            path = self._prefs_path()
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(json.dumps(self._prefs, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
        except OSError:
            pass

    def state(self):
        root = self._root if self._root and self._root.is_dir() else None
        manifest = root / '.xgfx' / 'xgfx_assets.json' if root else None
        return {'root': str(root) if root else None,
                'configs': ['xgfx_assets.json'] if manifest and manifest.is_file() else [],
                'recent': self._recent, 'version': core.__version__}

    def open_folder(self):
        import traceback
        try:
            folder = self._pick_folder()
        except Exception as e:
            raise RuntimeError(f'选择文件夹失败: {type(e).__name__}: {e}\n{traceback.format_exc()}') from e
        return self.open_project(folder) if folder else None

    def _dialog_directory(self):
        # 项目可能已被移动、删除，或所在磁盘已断开。不能把失效路径传给选择器。
        candidates = ([self._root, *self._root.parents] if self._root else [])
        candidates.extend([Path.home(), Path(sys.executable).parent])
        for candidate in candidates:
            try:
                if candidate.is_dir(): return str(candidate)
            except OSError:
                continue
        return ''

    def _pick_folder(self):
        import webview
        # Windows 后端使用 IFileDialog 的文件夹模式，支持地址栏、搜索和快捷访问。
        paths = self._window.create_file_dialog(
            webview.FileDialog.FOLDER,
            directory=self._dialog_directory(),
            allow_multiple=False,
        )
        return paths[0] if paths else None

    def open_project(self, path):
        import traceback
        try:
            root = Path(path).resolve()
            if not root.is_dir(): raise ValueError('项目文件夹不存在')
            self._root = root
            self._recent = [str(root)] + [p for p in self._recent if p != str(root)][:9]
            self._prefs['recent'] = self._recent
            self._save_prefs()
            return self.state()
        except Exception as e:
            raise RuntimeError(f'打开项目失败: {type(e).__name__}: {e}\n{traceback.format_exc()}') from e

    def load_prefs(self):
        return self._prefs

    def save_prefs(self, data):
        if not isinstance(data, dict): return False
        self._prefs.update(data)
        self._recent = list(self._prefs.get('recent', self._recent))
        self._save_prefs()
        return True

    def initialize(self):
        if self._root is None: raise ValueError('请先打开图片文件夹')
        core.init_project(self._root)
        return self.state()

    def _manifest(self, name):
        if self._root is None or name != 'xgfx_assets.json': raise ValueError('无效配置文件')
        return self._root / '.xgfx' / 'xgfx_assets.json'

    def load(self, name):
        return json.loads(self._manifest(name).read_text(encoding='utf-8-sig'))

    def save(self, name, data):
        if not isinstance(data.get('assets'), list): raise ValueError('配置缺少 assets 列表')
        core._atomic_text(self._manifest(name), json.dumps(data, ensure_ascii=False, indent=2)+'\n')
        return True

    def source_sizes(self, sources):
        if self._root is None: return {}
        sizes = {}
        for source in sources:
            try:
                sizes[source] = (self._root / '.xgfx' / source).resolve().stat().st_size
            except OSError:
                sizes[source] = None
        return sizes

    def source_info(self, sources):
        """Read lightweight source metadata once; previews must not rebuild every asset."""
        if self._root is None: return {}
        info = {}
        for source in sources:
            path = (self._root / '.xgfx' / source).resolve()
            try:
                with Image.open(path) as opened:
                    width, height = opened.size
                    image_format = opened.format or path.suffix.lstrip('.').upper()
                    has_alpha = 'A' in opened.getbands() or 'transparency' in opened.info
                info[source] = {'bytes': path.stat().st_size, 'width': width, 'height': height,
                                'format': image_format, 'has_alpha': has_alpha}
            except (OSError, ValueError):
                info[source] = None
        return info

    def _source_path(self, source):
        if self._root is None: raise ValueError('请先打开图片文件夹')
        project = self._root.resolve()
        path = (project / '.xgfx' / str(source)).resolve()
        try: path.relative_to(project)
        except ValueError as exc: raise ValueError('图片路径不在项目文件夹内') from exc
        if not path.is_file(): raise ValueError('图片文件不存在')
        return path

    def rename_source(self, source, new_stem):
        path = self._source_path(source)
        stem = str(new_stem).strip()
        if not stem or stem in {'.', '..'}: raise ValueError('文件名不能为空')
        if re.search(r'[<>:"/\\|?*\x00-\x1f]', stem) or stem[-1:] in {' ', '.'}:
            raise ValueError('文件名包含 Windows 不允许的字符')
        target = path.with_name(stem + path.suffix)
        if target.exists() and target != path: raise ValueError('同名文件已经存在')
        path.rename(target)
        return Path(os.path.relpath(target, self._root / '.xgfx')).as_posix()

    def reveal_source(self, source):
        path = self._source_path(source)
        if os.name != 'nt': raise ValueError('当前系统不支持资源管理器定位')
        subprocess.Popen(['explorer.exe', '/select,', str(path)])
        return True

    def scan(self, data):
        return core.scan_project(self._root, data)

    def choose_output(self):
        import webview
        paths = self._window.create_file_dialog(webview.FileDialog.FOLDER)
        if not paths: return None
        try: return Path(os.path.relpath(paths[0], self._root / '.xgfx')).as_posix()
        except ValueError: return paths[0]

    def _convert_entry(self, data, index):
        entry = core.effective_image_settings(data, data['assets'][int(index)])
        source = (self._root / '.xgfx' / entry['source']).resolve()
        c = core.AssetConfig(name=entry['name'], source=source, image_type=entry.get('type', 'MIRROR'),
            storage=entry.get('storage', 'MCU'), color_format=entry.get('color_format', 'RGB565'),
            byte_order=data.get('defaults', {}).get('byte_order', 'little'), bpp=int(entry.get('bpp', 0)),
            alpha_bpp=int(entry.get('alpha_bpp', 0)), palette=tuple(entry.get('palette', [])),
            quantization=entry.get('quantization', 'DOMINANT'),
            background_color=entry.get('background_color'),
            flash_address=core._integer(entry['flash_address']) if entry.get('flash_address') is not None else None)
        return c, core.AssetBuildService().convert(c, allow_auto_flash='flash' in data)

    def preview(self, data, index):
        index = int(index)
        source = (self._root / '.xgfx' / data['assets'][index]['source']).resolve()
        with Image.open(source) as opened:
            original_format = opened.format or source.suffix.lstrip('.').upper()
            original = opened.convert('RGBA')
        c, result = self._convert_entry(data, index)
        response = {'original': png_url(original), 'width': original.width, 'height': original.height,
                    'original_format': original_format, 'source_bytes': source.stat().st_size,
                    'errors': [d.message for d in result.diagnostics]}
        if result.succeeded:
            a = result.assets[0]
            response.update(converted=png_url(render_asset(a)), bytes=a.total_size,
                            palette=[core.device_to_rgb(v, c.color_format) for v in a.palette_values])
        return response

    def build(self, name, data):
        with self._lock:
            if self._progress.get('running'): raise ValueError('正在生成，请稍候')
            self.save(name, data)
            self._progress = {'running': True, 'done': 0, 'total': len(data['assets']), 'name': '准备生成'}
            manifest = self._manifest(name)
        def run():
            try:
                def report(name, done, total):
                    self._progress = {'running': True, 'name': name, 'done': done, 'total': total}
                result = core.AssetBuildService().build(manifest, progress=report)
                self._progress = {'running': False, 'ok': result.succeeded, 'count': len(result.assets),
                                 'errors': [f'{d.asset or "项目"}: {d.message}' for d in result.diagnostics]}
            except Exception as exc:
                self._progress = {'running': False, 'ok': False, 'errors': [str(exc)]}
        threading.Thread(target=run, daemon=True).start()
        return True

    def build_status(self):
        return self._progress

    def window_minimize(self):
        if self._window: self._window.minimize()

    def window_toggle_maximize(self):
        if not self._window: return
        from System import Action
        from System.Windows.Forms import FormWindowState
        form = self._window.native
        def toggle():
            form.WindowState = (FormWindowState.Normal
                if form.WindowState == FormWindowState.Maximized else FormWindowState.Maximized)
        form.Invoke(Action(toggle))

    def window_start_drag(self):
        if not self._window: return
        import ctypes
        from ctypes import wintypes
        from System import Action
        from System.Drawing import Point
        from System.Windows.Forms import Cursor, FormWindowState
        form = self._window.native
        def drag():
            user32 = ctypes.WinDLL('user32', use_last_error=True)
            user32.GetAsyncKeyState.argtypes = [ctypes.c_int]
            user32.GetAsyncKeyState.restype = ctypes.c_short
            if not user32.GetAsyncKeyState(1) & 0x8000: return
            pos = Cursor.Position
            if form.WindowState == FormWindowState.Maximized:
                ratio = max(0.0, min(1.0, (pos.X - form.Left) / max(1, form.Width)))
                offset_y = max(0, min(pos.Y - form.Top, 30))
                form.WindowState = FormWindowState.Normal
                form.Location = Point(int(pos.X - form.Width * ratio), pos.Y - offset_y)
            user32.ReleaseCapture()
            user32.SendMessageW.argtypes = [wintypes.HWND, wintypes.UINT, ctypes.c_size_t, ctypes.c_ssize_t]
            user32.SendMessageW.restype = ctypes.c_ssize_t
            # 交给 Windows 原生移动循环，保留跨屏移动与贴边行为。
            user32.SendMessageW(form.Handle.ToInt64(), 0xA1, 2, 0)
        form.Invoke(Action(drag))

    def window_close(self):
        self._allow_close = True
        if self._window: self._window.destroy()


def _resource_dir():
    import sys
    base = getattr(sys, '_MEIPASS', None)
    return Path(base) if base else Path(__file__).resolve().parent


def launch(root=None):
    import webview
    api = Api(root)
    res = _resource_dir()
    html = (res / 'index.html').read_text(encoding='utf-8')
    logo = (res / 'logo.svg').read_bytes()
    html = html.replace('__LOGO__', 'data:image/svg+xml;base64,'+base64.b64encode(logo).decode('ascii'))
    window = webview.create_window('XGFX Assets', html=html, js_api=api, width=1280, height=840,
                                  min_size=(960, 620), background_color='#000000', frameless=True, easy_drag=False,
                                  confirm_close=False)
    api._window = window
    def request_close():
        if api._allow_close or os.environ.get('XGFX_SMOKE'): return True
        # closing 回调等待返回，另开线程调用页面，避免与窗口 UI 线程互相等待。
        threading.Thread(target=lambda: window.evaluate_js(
            'window.requestClose && window.requestClose()'), daemon=True).start()
        return False
    window.events.closing += request_close
    def sync_window_state(*args):
        from System.Windows.Forms import FormWindowState
        maximized = window.native.WindowState == FormWindowState.Maximized
        window.evaluate_js('window.updateWindowState && window.updateWindowState(' + ('true' if maximized else 'false') + ')')
    window.events.maximized += sync_window_state
    window.events.restored += sync_window_state
    window.events.loaded += sync_window_state
    def smoke():
        import time
        outcome = {}
        try:
            for _ in range(100):
                time.sleep(.2)
                outcome = window.evaluate_js('({ready:!!window.pywebview, count:data?.assets?.length||0, selected, original:!!document.getElementById("original").naturalWidth, converted:!!document.getElementById("converted").naturalWidth})')
                if outcome.get('converted'): break
            if not outcome.get('converted'): raise RuntimeError(str(outcome) + str(window.evaluate_js('document.body.innerText')))
            window.evaluate_js('changeZoom(1.25); pan={x:30,y:20}; render();')
            if os.environ.get('XGFX_SCREENSHOT'):
                import time as _t
                _t.sleep(0.8)  # let webview finish painting after first frame
                window.evaluate_js('render();')
                _t.sleep(0.3)
                try:
                    window.minimize(); _t.sleep(0.15); window.restore(); _t.sleep(0.2)
                except Exception: pass
                from PIL import ImageGrab
                ImageGrab.grab().save(os.environ['XGFX_SCREENSHOT'])
            outcome['ok'] = True
        except Exception as exc: outcome = {'ok': False, 'error': str(exc)}
        Path(os.environ['XGFX_SMOKE']).write_text(json.dumps(outcome), encoding='utf-8')
        window.destroy()
    webview.start(smoke if os.environ.get('XGFX_SMOKE') else None, gui='edgechromium', private_mode=True)
    return 0


if __name__ == '__main__':
    import sys
    raise SystemExit(launch(sys.argv[1] if len(sys.argv)>1 else None))
