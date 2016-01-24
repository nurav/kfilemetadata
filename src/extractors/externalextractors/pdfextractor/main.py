#!/usr/bin/python

import sys
import subprocess
import os.path
import os

print("Hello")

inp = sys.stdin.read()
print(inp)

try:
    import simplejson as json
except ImportError:
    import json

def help():
    print("KFileMetaData extractor for executable files")
    print("")
    print("Usage:", os.path.basename(__file__), "[--mimetypes | filename]")
    print("\t--mimetype : List the mimetypes for the files this extractor can process")
    print("\tfilename   : Process the given filename")

def mimetypes():
    print("application/x-executable")

def extract(fileName):
    if os.path.exists(fileName):
        data = {
            "comment" : subprocess.check_output("file %s" % fileName, shell = True).decode("UTF-8").strip(),
            "invalid" : "invalid data"
        }
        print(json.dumps(data))

if __name__ == "__main__":
    if len(sys.argv) != 2:
        help()
    elif sys.argv[1] == "--mimetypes":
        mimetypes()
    else:
        extract(sys.argv[1])
