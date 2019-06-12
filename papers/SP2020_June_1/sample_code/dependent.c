unsigned char key = input();
// key = [0 ... 255]
if(key = 128)
    A(); // branch 1
else if (key < 64)
    B(); // branch 2
else if (key < 128)
    C(); // branch 3
else
    D(); // branch 4