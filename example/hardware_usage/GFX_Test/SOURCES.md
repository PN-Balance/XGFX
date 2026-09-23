# 测试图片来源

本目录只提交为 172×320 屏幕缩放后的测试副本；`source_images/selected/` 中的高分辨率原图被 `.gitignore` 排除，避免仓库膨胀。运行 `prepare_assets.py` 会从本地原图重新生成全部测试副本和资源。

| 内容 | 来源 |
|---|---|
| 山湖、长城、圣托里尼、小猫、金毛、透明猫、透明狗、Wikipe-tan | Wikimedia Commons 对应文件页；原图仅用于个人硬件测试 |
| 金毛犬 | <https://commons.wikimedia.org/wiki/File:Portrait_of_a_golden_retriever_on_the_grass.jpg> |
| 《千与千寻》测试帧 | Studio Ghibli 官方图库，`https://www.ghibli.jp/gallery/chihiro001.jpg` |
| 1–5 BPP 笑脸图案 | `prepare_assets.py` 自动绘制 |

提交的派生图只用于回归测试。若发布演示包，应再次核对每个 Commons 文件页所列许可，并移除不适合再分发的素材。
