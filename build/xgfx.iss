; XGFX installer for Inno Setup 6
; Build with:  ISCC.exe build\xgfx.iss
; Output:      build\installer\XGFX-Setup.exe
; Requires:    Inno Setup 6 (https://jrsoftware.org/isdl.php)

#define XGFXVersion "1.2.0"
#define XGFXPublisher "XGFX Project"

[Setup]
AppName=XGFX
AppId=XGFX
AppVersion={#XGFXVersion}
AppPublisher={#XGFXPublisher}
AppPublisherURL=https://github.com/
DefaultDirName={autopf}\XGFX
DefaultGroupName=XGFX
DisableProgramGroupPage=yes
OutputDir=dist\installer
OutputBaseFilename=XGFX-Setup
Compression=lzma2/ultra64
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
ArchitecturesAllowed=x64compatible
UninstallDisplayIcon={app}\xgfx.exe
WizardStyle=modern
SetupIconFile=gui_src\xgfx.ico
ChangesEnvironment=yes
PrivilegesRequired=admin
UsePreviousAppDir=yes
CloseApplications=yes
RestartApplications=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "addtopath"; Description: "把 xgfx 加入系统 PATH"; GroupDescription: "环境变量:"
Name: "desktopicon"; Description: "在桌面创建 XGFX Assets 快捷方式"; GroupDescription: "快捷方式:"

[Files]
; console command: xgfx.exe + dependencies
Source: "dist\xgfx_cli_dist\xgfx\*"; DestDir: "{app}"; Flags: recursesubdirs ignoreversion
; GUI app: XGFXAssets.exe + dependencies
Source: "dist\xgfx_desktop_dist\XGFXAssets\*"; DestDir: "{app}\gui"; Flags: recursesubdirs ignoreversion
; pure-python script, copied into each project's .xgfx/ on init so users can double-click to build
Source: "..\tools\xgfx_script\xgfx_asset.py"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; 显式指定 IconFilename，避免个别系统上快捷方式回退到默认图标。
Name: "{group}\XGFX Assets"; Filename: "{app}\gui\XGFXAssets.exe"; IconFilename: "{app}\gui\XGFXAssets.exe"; Comment: "XGFX 图片资源 GUI 工作台"
Name: "{group}\XGFX 命令行"; Filename: "{cmd}"; Parameters: "/k cd /d {app}"; IconFilename: "{app}\xgfx.exe"; Comment: "打开终端用 xgfx init / xgfx build"
Name: "{group}\卸载 XGFX"; Filename: "{uninstallexe}"
Name: "{autodesktop}\XGFX Assets"; Filename: "{app}\gui\XGFXAssets.exe"; IconFilename: "{app}\gui\XGFXAssets.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\gui\XGFXAssets.exe"; Description: "立即启动 XGFX Assets"; Flags: nowait postinstall skipifsilent

[Code]
function NeedsAddPath(P: string): Boolean;
var
  CurrentPath: string;
  SearchPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', CurrentPath) then
    Result := True
  else begin
    SearchPath := ';' + UpperCase(CurrentPath) + ';';
    Result := Pos(';' + UpperCase(P) + ';', SearchPath) = 0;
  end;
end;

procedure EnvAddPath(P: string);
var
  CurrentPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', CurrentPath) then
    CurrentPath := '';
  if CurrentPath <> '' then
    CurrentPath := CurrentPath + ';';
  RegWriteExpandStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', CurrentPath + P);
end;

procedure EnvRemovePath(P: string);
var
  CurrentPath: string;
  P_upper: string;
  SearchPath: string;
  Pos1: Integer;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', CurrentPath) then
    Exit;
  P_upper := UpperCase(P);
  SearchPath := ';' + UpperCase(CurrentPath) + ';';
  Pos1 := Pos(';' + P_upper + ';', SearchPath);
  if Pos1 > 0 then begin
    Delete(CurrentPath, Pos1, Length(P) + 1);
    if (Length(CurrentPath) > 0) and (CurrentPath[1] = ';') then
      Delete(CurrentPath, 1, 1);
    RegWriteExpandStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', CurrentPath);
  end;
end;

function InitializeSetup(): Boolean;
begin
  Result := True;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssPostInstall) and WizardIsTaskSelected('addtopath') and NeedsAddPath(ExpandConstant('{app}')) then
    EnvAddPath(ExpandConstant('{app}'));
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
    EnvRemovePath(ExpandConstant('{app}'));
end;
