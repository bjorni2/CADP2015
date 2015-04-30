

void * enter_v(void * argp)
{
    pthread_detach(pthread_self());


    try_cross(2);
    return NULL;
}
