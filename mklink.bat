IF EXIST "%1\styles\" move "%1\styles" "%1\styles-backup"
IF EXIST "%1\platformthemes\" move "%1\platformthemes" "%1\platformthemes-backup"
mklink /D "%1\styles" "%~dp0\styles"
mklink /D "%1\platformthemes" "%~dp0\platformthemes"
