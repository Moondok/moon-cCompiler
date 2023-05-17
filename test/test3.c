int main()
{
    int arr[5];
    
    // initialize the array;
    for(int i=0;i<6;++i)
    {

        if(i>=5)
        {
            int c=0; //redundant variable
            break;
        }
        arr[i]=5-i;
    }

    //bubble sort
    
    for(int i=0;i<5;i++) // same variable name i , we can handle that!
    {
        int j=0;
        for(j;j<5-i-1;j++)
        {
            if(arr[j]>arr[j+1])
            {
                int tmp=arr[j];
                arr[j+1]=arr[j];
                arr[j]=tmp;
            }
        }
    }
    return 0;
}