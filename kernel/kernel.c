
void main(void) {
    *((unsigned char*)0xB8000) = 'N';
    *((unsigned char*)0xB8001) = 0xf;
}
