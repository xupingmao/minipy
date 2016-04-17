
import logging
from test import *

logging.info('WHILE STATEMENT TEST')

logging.info('Global Scope Test Start')

logging.info('test loop from 0 to 10')
i = 0
while i < 10:
    logging.info('i=', i)
    i += 1
assertEquals(i, 10)
logging.info('Global Scope Test End')

def whilefunc():
    logging.info('Local Scope Test Start')
    i = 0
    while i < 10:
        logging.info('i=', i)
        i+=1
    assertEquals(i, 10)
    logging.info('Local Scope Test End')
    
whilefunc()