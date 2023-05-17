float add_float(float a1, float a2) //test float
{
    return a1+a2;
}

int main()
{
    int i=0,t=2;
    float f1=2.0;
    float f2=3.0;

    float re=add_float(f1,f2);

    do //test do-while
    {
        t+i;
        /* code */
        i++;
    } while (i<3);
    
    return 0;
}