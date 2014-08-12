##########
#
#  Nullsoft Scriptable Install System
#
#  Install script by insomnia
#
#    musikCube (c) Casey Langen 2002-2006
#
#    Resources: NSIS Wiki / Forum, Internet
#
#____ Language

  !insertmacro MUI_LANGUAGE "German"

#____ Install Options

  LangString IO_Header ${LANG_GERMAN} "Verkn�pfungen einrichten"
  LangString IO_SubHeader ${LANG_GERMAN} "Programmsymbole erstellen"
  LangString IO_ProgIcons ${LANG_GERMAN} "Programmsymbole erstellen f�r musikCube:"  
  LangString IO_DeskIcon ${LANG_GERMAN} "Auf dem Desktop"
  LangString IO_SMPIcon ${LANG_GERMAN} "Im Startmen�"    
  LangString IO_QLIcon ${LANG_GERMAN} "In der Schnellstartleiste"
  LangString IO_PluginsHeader ${LANG_GERMAN} "Zielverzeichnis ausw�hlen"
  LangString IO_PluginsSubHeader ${LANG_GERMAN} "W�hlen Sie das Verzeichnis aus, in das $(^NameDA) installiert werden soll."
  LangString IO_PluginsExplanation ${LANG_GERMAN} "$(^NameDA) wird in das unten angegebene Verzeichnis installiert.\n\
  Dies muss dasselbe Verzeichnis sein, in dem auch musikCube installiert wurde!"
  LangString IO_PluginsTasks ${LANG_GERMAN} "Aufgaben"
  LangString IO_PluginsAssociate ${LANG_GERMAN} "Verkn�pfe ${PLUGIN_EXT} mit musikCube"

#____ Message Boxes

  LangString MB_UnsupportedOS ${LANG_GERMAN} "${PRODUCT_NAME} unterst�tzt Windows 2000, XP, 2003 Server.$\n$\n\
  Wir bieten KEINE Unterst�tzung f�r Windows 95, 98, oder ME an."
  LangString MB_AlreadyInstalledIS ${LANG_GERMAN} "Eine �ltere Version von ${PRODUCT_NAME} (<= 0.92.5) ist bereits installiert.$\n$\n\
  Dr�cken Sie 'OK' um die vorherige Version zu entfernen oder 'Abbrechen' um diese Installation abzubrechen."
  LangString MB_AlreadyInstalledNSIS ${LANG_GERMAN} "Eine �ltere Version von ${PRODUCT_NAME} (> 0.92.5 und < 1.0) ist bereits installiert.$\n$\n\
  Dr�cken Sie 'OK' um die vorherige Version zu entfernen oder 'Abbrechen' um diese Installation abzubrechen."
  LangString MB_OldFiles ${LANG_GERMAN} "Es gibt einige alten Dateien im Zielverzeichnis, die vom Installer entfernt werden m�ssen.$\n$\n\
  Dr�cken Sie 'OK' um diese Dateien zu entfernen oder 'Abbrechen' um das Installationsverzeichnis zu �ndern."  
  LangString MB_DetectionError ${LANG_GERMAN} "Der Installer war nicht in der Lage festzustellen ob Sie mit dem Internet verbunden sind.$\n\
  Wenn Sie sich sicher sind, dass dies der Fall ist, dr�cken Sie 'Ja', anderenfalls dr�cken Sie 'Nein'. Letzeres wird die Plugin-Installation abbrechen."
  LangString MB_OfflineError ${LANG_GERMAN} "Sie sind offline. Um das Format-Plugin zu installieren, m�ssen Sie mit dem Internet verbunden sein!$\n\
  Bitte stellen Sie eine Verbindung her und dr�cken Sie 'Wiederholen'! Wenn Sie das Plugin nicht installieren m�chten, dr�cken Sie 'Abbrechen'."

#____ Descriptions
#
#  LangString IT_Minimal ${LANG_GERMAN} "Minimal"
#  LangString IT_Full ${LANG_GERMAN} "Vollst�ndig"
#  LangString IT_Custom ${LANG_GERMAN} "Benutzerdefiniert"
#  LangString Desc_Main ${LANG_GERMAN} "Installiert musikCube und alle ben�tigten Dateien."
#  LangString Desc_Plugins ${LANG_GERMAN} "L�dt zus�tzliche Format-Plugins herunter."
#  LangString Desc_APE ${LANG_GERMAN} "APE Format-Plugin."
#  LangString Desc_FLAC ${LANG_GERMAN} "FLAC Format-Plugin."
#  LangString Desc_M4A ${LANG_GERMAN} "MPEG 4 Audio Format-Plugin."
#  LangString Desc_MOD ${LANG_GERMAN} "MOD Format-Plugin."
#  LangString Desc_WAV ${LANG_GERMAN} "WAV Format-Plugin."
#  LangString Desc_WMA ${LANG_GERMAN} "Widows Media Audio Format Plugin."
#  LangString Desc_MPC ${LANG_GERMAN} "MusePack Format-Plugin."
#  LangString Desc_WV ${LANG_GERMAN} "WavePack Format-Plugin."
#  LangString Desc_InternetRequired ${LANG_GERMAN} "(Internetverbindung ben�tigt)"

#____ Detail Prints

  LangString DP_BackupCreated ${LANG_GERMAN} "Backup erstellt"
  LangString DP_ExtAssociated ${LANG_GERMAN} "Dateierweiterung verkn�pft"
  LangString DP_FCCreated ${LANG_GERMAN} "Dateiklasse erstellt"
  LangString DP_ExtRemoved ${LANG_GERMAN} "Dateierweiterung entfernt"
  LangString DP_BackupRestored ${LANG_GERMAN} "Backup wiederhergestellt"
  LangString DP_FCRemoved ${LANG_GERMAN} "Dateiklasse entfernt"
 
#____ NSISdl
#
#  LangString NSISdl_Down ${LANG_GERMAN} "Lade %s herunter"
#  LangString NSISdl_Conn ${LANG_GERMAN} "Verbinde ..."
#  LangString NSISdl_Sec ${LANG_GERMAN} "Sekunde"
#  LangString NSISdl_Min ${LANG_GERMAN} "Minute"
#  LangString NSISdl_Hour ${LANG_GERMAN} "Stunde"
#  LangString NSISdl_Plural ${LANG_GERMAN} "n"
#  LangString NSISdl_Prog ${LANG_GERMAN} "%dkB (%d%%) von %dkB @ %d.%01dkB/s"
#  LangString NSISdl_Remain ${LANG_GERMAN} " (%d %s%s verbleibend)"
