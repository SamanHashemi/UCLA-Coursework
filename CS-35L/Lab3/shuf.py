#!/usr/bin/python                                                              

import random, sys
from optparse import OptionParser

class shuffle:
    def __init__(self, filename):
        # If the file is nothing
        if filename == "-" and not filename:
            self.lines = sys.stdin.readlines()
            random.shuffle(self.lines)
        # If the file is a list (echo is on)
        elif isinstance(filename, list):
            self.lines = []
            for word in filename:
                self.lines.append(word+'\n')
        else: # File is normal do your thing
            f = open(filename, 'r')
            self.lines = f.readlines()
            random.shuffle(self.lines)
            f.close()
            
    # Get a random line and return it
    def chooseline(self):
        return random.choice(self.lines)

def main():
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]... FILE                                      
  or:  %prog -e [OPTION]... [ARG]...                                           
Write a random permutation of the input lines to standard output.              

With no FILE, or when FILE is -, read standard input."""

    parser = OptionParser(version=version_msg, usage=usage_msg)

    # Add options for -e (echo) command
    parser.add_option("-e", "--echo",
                      action="store_true", dest="echo", default=False,
                      help="treat each ARG as an input line")

    # Add options for -n (number of repeats) command
    parser.add_option("-n", "--head-count",
                      action="store", dest="count",
                      help="output at most COUNT lines")

    # Add options for the -r (repeat) command
    parser.add_option("-r", "--repeat",
                      action="store_true", dest="repeat", default=False,
                      help="output lines can be repeated")

    options, args = parser.parse_args(sys.argv[1:])

    # Assign variables from above for easy access
    repeat = bool(options.repeat)
    echo = bool(options.echo)

    # If the echo is nothing the file name is arg1
    if (echo == False):
        input_file = args[0]
    else:  # If the echo is true then add the echo words 
        input_file = []
        for arg in args:
            input_file.append(arg)

    try:  # Get the number of repeats if specified
        count = int(options.count)
        infinite = False
    except:  # Nothing specified get number of lines
        infinite = True
        if echo == False:
            count = sum(1 for line in open(input_file))
        else:
            count = len(args)
            
    # If the count was invalid output error try again
    if count < 0:
        parser.error("invalid line count: {0}".format(options.count))
        count = sum(1 for line in open(input_file))

    # Try to create the shuffle class object
    try:        
        generator = shuffle(input_file)

        # Infinite Repeat. Don't. Stop. Printing.
        if repeat == True and infinite == True:
            while True:
                for line in generator.lines:
                    sys.stdout.write(line)
        # If the repeat is on and it is finite
        elif repeat == True and infinite == False:
            for i in range(0, count):
                sys.stdout.write(generator.chooseline())
        # Print only count number of times
        elif repeat == False and infinite == True:
            for i in range(0, count):
                sys.stdout.write(generator.lines[i])
        # No count specified print count number of times
        elif repeat == False and infinite == False:
            for i in range(0, count):
                sys.stdout.write(generator.lines[i])
        else: # Basically useless
            for index2 in input_file:
                sys.stdout.write(random.choice(input_file))
    except IOError as (errno, strerror):
        parser.error("I/O error({0}): {1}".format(errno, strerror))

if __name__ == "__main__":
    main()
