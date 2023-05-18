# moon-cC : a toy compiler with dual backends
## Brief intro
This is a toy C compiler .It can transform a subset of ```C language``` into ```Mips``` assembly.
The C language standard is [here](https://www.quut.com/c/ANSI-C-grammar-y-1999.html#labeled-statement).

## features
### generalizability
moon-cC supports a large subset of ```C language```.It supports two data types: ```int``` and ```double``` for now. Besides , it supports arrays and function invoke.

### ability to detect errors
moon-cC can detect a number of ubiquitous errors when programming ,there are 5 cases and corresponding error message which will be output in ```errors.log```.More test files can be found in ```test``` folder.
+ undeclared variable
```
int main()
{
    int i=0;
    int c=3;
    if(c>2)
        x=i;// undeclared variable
}
```
```
SEMANTIC ERRORS:
some error occurs at line 6 column 9 : undefined variable x.
```
+ call a function before its definition
```
int main()
{
    int a=3;
    int b=2;
    int c=0,d=func(a,b);
}
int func(int a,int b)
{
    return a+b;
}
```
```
SEMANTIC ERRORS:
some error occurs at line 6 column 18 : undefined function func
some error occurs at line 6 column 13 : must use same type to perform initialization.
```
+ misuse ```break``` in selection statement
```
int main()
{
    int a=0,b=2;
    if(a==1)
        b=1;
    else
    {
        b=3;
        break;
    }
}
```
```
SEMANTIC ERRORS:
some error occurs at line 9 column 13 : can not break this scope.
```
+ the arguments passed are not match the parameters defined
```
int func(int a,double b)
{
    return 1;
}

int main()
{
    int a=1,b=2;
    int c;
    c=func(a,b);
    return 0;
}
```
```
SEMANTIC ERRORS:
some error occurs at line 11 column 12 : the type of argument you passed is not match the type you defined.
```
+ multiple definitions of one function
```
int return_number()
{
    int a;
    return 1;
}

int return_number()
{
    return 3+5;
}
int main()
{
    int c;
    int x;
    x=return_number();
}
```
```
SEMANTIC ERRORS:
some error occurs at line 9 column 3 : function return_number is already defined.
```



## what moon-cC do not support for now
+ pointer
+ goto
+ const
+ string


## This project is still in progress