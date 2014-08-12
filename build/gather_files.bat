rd /q /s output
mkdir output
mkdir output\formats
mkdir output\img
mkdir output\plugins

xcopy /S /Y input\*.* output 

copy bin\release\*.exe output
copy bin\release\*.dll output
copy bin\release\plugins\*.dll output\plugins
copy bin\release\formats\*.dll output\formats

xcopy /S /Y output\*.* bin







