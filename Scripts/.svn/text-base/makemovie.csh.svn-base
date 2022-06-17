set current = `pwd`
set list = `ls *.gif`

set QtExportSettings = $SLAQR_HOME/Par/ExportToPNG
set QtExportSettings = `echo $QtExportSettings | sed "s/\//\:/g" | sed "s/\://"`
echo $QtExportSettings
set current = `echo $current | sed "s/\//\:/g" | sed "s/\://"`
set infile = $current\:$list[1]
set outfile = $current\:last24hr
echo $infile
echo $outfile
rm last24hr

osascript <<ENDQT
set pictureFile to "$infile"
set outputFile to "$outfile"
set settingsFile to "$QtExportSettings"
tell application "QuickTime Player"
    open image sequence file pictureFile frames per second 15
    export document 1 to outputFile as QuickTime movie using  settings settingsFile 
    quit saving no
end tell 
ENDQT
