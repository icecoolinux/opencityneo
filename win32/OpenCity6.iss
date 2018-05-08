; ------------------------------------------------------------------------------
; -- OpenCity.iss --
;
; Last modified:
;	$Id: OpenCity6.iss 422 2009-10-11 17:31:11Z neoneurone $
; ------------------------------------------------------------------------------

[Setup]
AppCopyright=Copyright (C) 2009 by Duong Khang NGUYEN
AppName=OpenCityNeo 0.1.0 dev
AppPublisher=Icecool
AppPublisherURL=https://github.com/icecoolinux/opencityneo
AppVerName=OpenCityNeo 0.1.0 dev
AppVersion=OpenCityNeo 0.1.0 dev
Compression=lzma
DefaultDirName={pf}\OpenCityNeo
DefaultGroupName=OpenCityNeo
LicenseFile=..\COPYING
SolidCompression=yes
VersionInfoVersion=0.1.0
;WindowVisible=no

[Languages]
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"

[Files]
Source: "bin\Release\OpenCityNeo.exe"; DestDir: "{app}\bin";
Source: "bin\Release\*.dll"; DestDir: "{app}\bin"
Source: "..\config\*"; DestDir: "{app}\etc\opencityneo\config"; Excludes: ".svn"
Source: "..\graphism\*"; DestDir: "{app}\share\opencityneo\graphism"; Excludes: ".svn"; Flags: recursesubdirs createallsubdirs
Source: "..\sound\*"; DestDir: "{app}\share\opencityneo\sound"; Excludes: ".svn"
Source: "docs\*.txt"; DestDir: "{app}\share\opencityneo\docs";
Source: "docs\FAQ.txt"; DestDir: "{app}"
Source: "docs\README.txt"; DestDir: "{app}"; Flags: isreadme
; MSVC C runtime manifest
;Source: "Release\Microsoft.VC80.CRT.manifest"; DestDir: "{app}\bin"

[Dirs]
Name: "{userappdata}\opencityneo\"

[Icons]
Name: "{group}\OpenCityNeo - window"; Filename: "{app}\bin\OpenCityNeo.exe"; IconFilename: "{app}\share\opencityneo\graphism\icon\OpenCity32.ico"; Parameters: "-dd ""{app}\share\opencityneo"" -cd ""{app}\etc\opencityneo"" -glv"; WorkingDir: "{app}"
Name: "{group}\OpenCityNeo - fullscreen (autodetect mode)"; Filename: "{app}\bin\OpenCityNeo.exe"; IconFilename: "{app}\share\opencityneo\graphism\icon\OpenCity32.ico"; Parameters: "-dd ""{app}\share\opencityneo"" -cd ""{app}\etc\opencityneo"" -fs -glv"; WorkingDir: "{app}"
Name: "{group}\Configuration file"; Filename: "{app}\etc\opencity\config\opencity.xml"
Name: "{group}\Save folder"; Filename: "{userappdata}\opencityneo\"
Name: "{group}\Homepage"; Filename: "https://github.com/icecoolinux/opencityneo"
Name: "{group}\Documentation\FAQ in English"; Filename: "{app}\FAQ.txt"
Name: "{group}\Documentation\FAQ in Italian"; Filename: "{app}\share\opencityneo\docs\FAQ_it.txt"
Name: "{group}\Documentation\INSTALL in Italian"; Filename: "{app}\share\opencityneo\docs\INSTALL_it.txt"
Name: "{group}\Documentation\README in English"; Filename: "{app}\README.txt";
Name: "{group}\Documentation\README in Italian"; Filename: "{app}\share\opencityneo\docs\README_it.txt"
Name: "{group}\Documentation\README in Spanish"; Filename: "{app}\share\opencityneo\docs\README_es.txt"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}";

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Run]
Filename: "{win}\explorer.exe"; Parameters: "https://github.com/icecoolinux/opencityneo"; Flags: nowait skipifdoesntexist

[Code]
function InitializeSetup: Boolean;
var
  version: TWindowsVersion;
begin
  GetWindowsVersionEx(Version);

  // Print confirmation message for Windows after XP
  if (version.Major > 5) then
  begin
    if MsgBox('OpenCity may not work with your Windows installation, are you sure to continue ?',
      mbConfirmation, MB_YESNO) = IDNO then
      begin
        Result := False;
        Exit;
      end;
  end;

  Result := True;
end;
