import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "build" / "gui_src"))

from desktop import Api


class DesktopApiTests(unittest.TestCase):
    def test_rename_source_preserves_extension(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / ".xgfx").mkdir()
            source = root / "icons" / "old.name.png"
            source.parent.mkdir()
            source.write_bytes(b"png")
            api = Api(root)
            renamed = api.rename_source("../icons/old.name.png", "new name")
            self.assertEqual(renamed, "../icons/new name.png")
            self.assertTrue((root / "icons" / "new name.png").is_file())

    def test_rename_source_rejects_invalid_name(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / ".xgfx").mkdir()
            source = root / "image.png"
            source.write_bytes(b"png")
            api = Api(root)
            with self.assertRaises(ValueError):
                api.rename_source("../image.png", "bad/name")


if __name__ == "__main__":
    unittest.main()
