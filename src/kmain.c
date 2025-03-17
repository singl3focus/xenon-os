void kmain() {
   volatile unsigned int magic = 0xCAFEBABE; // Используем volatile
    (void)magic; // Гарантируем, что переменная не будет удалена
}
