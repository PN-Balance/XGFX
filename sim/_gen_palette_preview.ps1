# 用 .NET System.Drawing 生成四象限测试图预览 PNG
# 12x12 放大 20 倍到 240x240，四象限：左上红/右上绿/左下蓝/右下黑
Add-Type -AssemblyName System.Drawing

$W = 12; $H = 12; $SCALE = 20
$imgW = $W * $SCALE; $imgH = $H * $SCALE

$bmp = New-Object System.Drawing.Bitmap($imgW, $imgH)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.Clear([System.Drawing.Color]::FromArgb(128, 128, 128))

# 调色板 RGB888
$pal = @(
    [System.Drawing.Color]::FromArgb(0, 0, 0),       # 0 黑
    [System.Drawing.Color]::FromArgb(255, 0, 0),     # 1 红
    [System.Drawing.Color]::FromArgb(0, 255, 0),     # 2 绿
    [System.Drawing.Color]::FromArgb(0, 0, 255)      # 3 蓝
)

# 填充四象限
$brush0 = New-Object System.Drawing.SolidBrush($pal[1])  # 红
$brush1 = New-Object System.Drawing.SolidBrush($pal[2])  # 绿
$brush2 = New-Object System.Drawing.SolidBrush($pal[3])  # 蓝
$brush3 = New-Object System.Drawing.SolidBrush($pal[0])  # 黑

# 左上 红 (0,0) - (5,5)
$g.FillRectangle($brush0, 0, 0, 6 * $SCALE, 6 * $SCALE)
# 右上 绿 (6,0) - (11,5)
$g.FillRectangle($brush1, 6 * $SCALE, 0, 6 * $SCALE, 6 * $SCALE)
# 左下 蓝 (0,6) - (5,11)
$g.FillRectangle($brush2, 0, 6 * $SCALE, 6 * $SCALE, 6 * $SCALE)
# 右下 黑 (6,6) - (11,11)
$g.FillRectangle($brush3, 6 * $SCALE, 6 * $SCALE, 6 * $SCALE, 6 * $SCALE)

# 画象限分割线（白色）
$pen = New-Object System.Drawing.Pen([System.Drawing.Color]::White, 2)
$g.DrawLine($pen, 6 * $SCALE, 0, 6 * $SCALE, $imgH)  # 垂直
$g.DrawLine($pen, 0, 6 * $SCALE, $imgW, 6 * $SCALE)  # 水平

# 画外边框
$penB = New-Object System.Drawing.Pen([System.Drawing.Color]::Gray, 1)
$g.DrawRectangle($penB, 0, 0, $imgW - 1, $imgH - 1)

$out = "f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim\_palette_preview.png"
$bmp.Save($out, [System.Drawing.Imaging.ImageFormat]::Png)

$g.Dispose(); $bmp.Dispose()
Write-Host "已保存: $out ($imgW x $imgH)"
