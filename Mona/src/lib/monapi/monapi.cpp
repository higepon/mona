int user_start_impl();

/*----------------------------------------------------------------------
    entry point for application
----------------------------------------------------------------------*/
extern "C" int user_start()
{
    return user_start_impl();
}
