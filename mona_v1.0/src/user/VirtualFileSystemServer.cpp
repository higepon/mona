/*!
  \file  VirtualFileSystemServer
  \brief class file service

  Copyright (c) 2004 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2004/01/25 update:$Date$
*/

#include <userlib.h>
#include <UFAT12.h>

int main() {

    /* Message loop */
    for (;;) {

        MessageInfo info;

        /* receive */
        if (!Message::receive(&info)) {

            switch(info.header) {

            }


        }

    }
}
