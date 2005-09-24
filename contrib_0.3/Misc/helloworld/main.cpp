#include <monapi.h>


#define MAIN_2

#ifdef MAIN_1

#include <monapi2/Basic/String.h>

// これはうまく動く
int MonaMain(List<char*>* pekoe)
{
    monapi2::String message("with monapi2 by junjunn");
    printf("Hello World %s", message.getString());
    return 0;
}


#endif // MAIN_1

#ifdef MAIN_2

#include <monapi2/Basic/CollectionMap.h>

// access denied.address 0x00000000 eip = 0xA00012D8で落ちる
int MonaMain(List<char*>* pekoe)
{
    monapi2::IntMap<byte*> cache;
    return 0;
}

#endif
