#include <iostream>
using namespace std;


void MinHeapAdd(int *a,int n,int number) 
{    
    a[n]=number; 
    int i=n; 
    int j=(i-1)/2; 
    int tmp=a[i]; 
     
    while(j>=0) 
    { 
        if(a[j]<=tmp) 
            break; 
        else 
        { 
            a[i]=a[j]; 
            i=j; 
            j=(i-1)/2; 
        } 
    } 
    a[i]=tmp; 
}

void MinHeapFixDown(int *a,int m,int n) 
{ 
    int i=m; 
    int j=2*i+1; 
    int tmp=a[i]; 
    while(j<n) 
    { 
        if(j+1<n&&a[j]>a[j+1]) 
            j++; 
        if(a[j]>=tmp) 
            break; 
        else 
        { 
            a[i]=a[j]; 
            i=j; 
            j=2*i+1; 
        }
		for(int t = 0; t < n; t++)
			cout << a[t] << " ";
		cout << std::endl;
    } 
    a[i]=tmp; 
}

void MinHeapDelete(int *a,int n) 
{ 
    a[0]=a[n-1]; 
    MinHeapFixDown(a,0,n-1); 
}

void MakeMinHeap(int *a,int n) 
{ 
    for(int i=n/2-1;i>=0;i--)//注意 
        MinHeapFixDown(a,i,n);
}

void HeapSort(int *a,int n) 
{ 
    MakeMinHeap(a,n); 
    for(int i=n-1;i>=1;i--) 
    { 
        int tmp=a[0];a[0]=a[i];a[i]=tmp; 
        MinHeapFixDown(a,0,i); 
    } 
}
 
int main(int argc, char** argv)
{  
    int a[10]={36,30,18,40,32,45,22,50}; 
    HeapSort(a,8); 

/**
    for(int i=0;i<8;i++) 
        cout<<a[i]<<" "; 
    cout<<endl; 
    int aa[10]={36,30,18,40,32,45,22,50}; 
    MakeMinHeap(aa,8); 
    for(int i=0;i<8;i++) 
        cout<<aa[i]<<" "; 
    cout<<endl; 
    MinHeapAdd(aa,8,35); 
    for(int i=0;i<9;i++) 
        cout<<aa[i]<<" "; 
    cout<<endl; 
    MinHeapDelete(aa,9); 
    for(int i=0;i<8;i++) 
        cout<<aa[i]<<" "; 
    cout<<endl; 
**/    
	return 0;
}
