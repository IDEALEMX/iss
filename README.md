# iss
ideale's Simple Shell is a minimal shell made on the c language  

# supported featues:
(feature will end with a * if not yet added)  

execute commands  
execute commands with arguments  

custom *cd* command  
*iss* command used to apply changes in the .issrc file *

-- valid settings in ~/.issrc --  
path *path*
alias *name_to_alias* *program_to_execute*

-- the following require spaces inbetween commands --  
run programs secuentially with + (like ; in bash) *  
run next command only if first succeeds && *  
run next command only if first fails || *  

pipe stdout | *  
redirect stdout to a file > (overwrite) *  
redirect stdout to a file >> (append) *  

redirect a file as stdin < (overwrite) *  

redirect stderr to a file 2> (overwrite) *  
redirect stderr to a file 2>> (append) *  

-- shell keybinds --  
autocomplete *  
return to previous commands *  
control-l to clear screen *  

