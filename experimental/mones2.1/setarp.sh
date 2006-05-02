#! /bin/bash

IP=$( ipconfig | gawk 'BEGIN{FS="[. ]"} /Ethernet/{ if(match($0,/VMnet8/)){ FLAG=1 }else{ FLAG=0 } }
/Address/{ if( FLAG==1 ){ print $(NF-3)"."$(NF-2)"."$(NF-1)"."$NF+1} }')

gawk '/generatedAddress\ =/{ print $3 }' ${MONADIR}/tool/mkimg/mona.vmx |\
sed 's/"//g;s/:/-/g' | xargs arp -s $IP

arp -a