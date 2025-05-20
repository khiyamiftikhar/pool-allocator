# Turn off all logging
set logging enabled off
set logging debugredirect on
set trace-commands off
set logging redirect on

# Optimize packet handling
set remote memory-write-packet-size 5192
set remote memory-read-packet-size 5192
target remote :3333
mon reset halt

# Break at app_main and start tracing
b app_main
commands
  mon log_output E:/temp/openocd_log.txt
  mon esp sysview start file://E:/temp/sysview0.svdat file://E:/temp/sysview1.svdat
  mon esp sysview status  # Optional: check if trace started
  c
end

# Breakpoint at specific line to stop tracing
#b segger-sysview.c:147
#commands
 # mon esp sysview stop
  #mon esp sysview status  # Optional: confirm trace stopped

c
#end
