# Python 脚本用法示例

使用纯 Python 脚本生成图片资源，无需安装，只需 Python 3 + Pillow。

## 环境

```powershell
pip install pillow
```

## 用法

进入图片目录，直接运行脚本（不带任何参数）：

```powershell
cd images
python ../../../tools/xgfx_script/xgfx_asset.py
```

- 首次运行：扫描图片并创建 `.xgfx/xgfx_assets.json` 配置文件
- 编辑配置后再次运行同一命令：生成 C/H/BIN 资源文件

脚本同时会把自身复制一份到 `.xgfx/xgfx_asset.py`，开发过程中双击该文件即可快速重新构建。

## 输出

生成的 `gfx_assets.c`、`gfx_assets.h`、`gfx_assets.bin` 默认输出到当前目录，加入嵌入式工程即可使用。
