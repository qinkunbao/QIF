unsigned char k1, k2, k3;
...
t1 = T[k1 % 8];        // Leakage 1
t2 = T[k2 % 8];        // Leakage 2
t3 = T[k3 % 8];        // Leakage 3
bool flag = foo(t1, t2, t3);
if(flag) {
    if(k1 > 128)       // Leakage 4          
        A();
}
else {
    if(k2 > 128)       // Leakage 5
        B();
}
if(k1 + k2 + k3 > 128) // Leakage 6
     C();
