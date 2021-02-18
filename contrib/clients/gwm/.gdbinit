set verbose off
define w
where
end

break stop_if_in_dbx
break wool_break
break GWM_FatalSignalHandler
break XFatalHandler

directory xpm
handle 8 stop nopass
define rf
set GWM_re_init_PointerRoot()
end
