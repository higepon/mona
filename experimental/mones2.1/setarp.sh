#! /bin/bash
gawk '/generatedAddress\ =/{
  print $3
}' ${MONADIR}/tool/mkimg/mona.vmx |\
sed 's/"//g;s/:/-/g' | xargs arp -s 192.168.10.10 
arp -a