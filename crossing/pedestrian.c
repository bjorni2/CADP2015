

void * enter_p(void * argp)
{
    pthread_detach(pthread_self());



    try_cross(1);
    retrurn NULL;
}
