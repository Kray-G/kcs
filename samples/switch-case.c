int switch_func(int x)
{
    switch (x) {
    case 1: return 1;
    case 2: return 2;
    case 3: return 3;
    case 4: return 4;
    case 5: return 5;
    case 6: return 6;
    case 7: return 7;
    case 8: return 8;
    default:
        break;
    }
    return 10;
}

int main(void)
{
    for (int i = -9; i < 10; ++i) {
        printf("[%2d] switch_func(%d) = %d\n", i, i, switch_func(i));
    }
    return 0;
}

