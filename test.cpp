/*
 * test.c
 * Zhaonian Zou
 * Harbin Institute of Technology
 * Jun 22, 2011
 */

#include <stdlib.h>
#include <stdio.h>
#include "extmem.hpp"

using namespace ExtMem;
int main(int argc, char **argv)
{
    Buffer buf(20,8);


    int i = 0;
    /* Get a new block in the buffer */
    auto mb_block = buf.get_block();
    auto block = std::move(mb_block.value());


    /* Fill data into the block */
    for (i = 0; i < 8; i++)
        block[i] = 'a' + i;

    /* Write the block to the hard disk */
    if (buf.dump_block(std::move(block),31415926) != 0)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
    auto b2 = buf.read_block(31415926);
    /* Read the block from the hard disk */
    if (!b2)
    {
        perror("Reading Block Failed!\n");
        return -1;
    }

    /* Process the data in the block */
    for (i = 0; i < 8; i++)
        printf("%c", (*b2)[i]);

    printf("\n");
    printf("# of IO's is %d\n", buf.iotimes()); /* Check the number of IO's */

    return 0;
}

