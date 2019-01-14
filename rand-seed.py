import sys
import random
import hashlib

#Takes a cmd line argument which sets the randomization range, then creates a MD5 hash of it.
def randomMD5Generator():
    hash = str(random.randint(0, int(sys.argv[1])))
    return hashlib.md5(hash.encode('utf-8')).hexdigest()


if __name__ == '__main__':
    sys.stdout.write(randomMD5Generator())