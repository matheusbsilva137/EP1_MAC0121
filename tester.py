#!/usr/bin/env python3


"""MAC0121 EP1's unit test."""


# python3.6 ./turtledorm.c ./executaveis/lightout-macos 10258862 vitorssrg@usp.br


import math
import numpy as np
import os
import platform
import random
import re
import subprocess
import sys
import tempfile
import time

from datetime import datetime, timedelta
from glob import glob


__author__      = 'Vitor Santa Rosa Gomes'
__copyright__   = 'Copyright 2018, Vitor Santa Rosa Gomes'
__credits__     = ['Vitor Santa Rosa Gomes']
__license__     = 'MIT'
__version__     = '1.0'
__maintainer__  = 'Vitor Santa Rosa Gomes'
__email__       = 'vitorssrg@usp.br'
__status__      = 'Development'



#region formating
def escape(s):
    return repr(s)[1:-1].replace('"', '\\"')

def parse_time(s):
    regex = re.compile(r'\b(((?P<hours>\d+)\:)?(?P<minutes>\d+)\:)?'
                         r'(?P<seconds>\d+)'
                         r'(\.(?P<milliseconds>\d+))?\b')
    groups = regex.match(s)

    if not groups:
        regex = re.compile(r'\b((?P<seconds>\d+)s'
                              r'|(?P<minutes>\d+)m'
                              r'|(?P<hours>\d+)h)\b')
        groups = regex.match(s)

    if not groups:
        return
    groups = groups.groupdict()
    
    return timedelta(**{key: int(value) 
                        for key, value in groups.items() 
                        if value is not None}).total_seconds()

def format_time(s):
    return f'{s:5.2f}s'

def parse_memory(b):
    predecs = list(map(lambda i: i.lower(), ['B', 'kB',  'MB',  'GB',  'TB' ]))
    prebins = list(map(lambda i: i.lower(), ['B', 'KiB', 'MiB', 'GiB', 'TiB']))

    search = re.compile(r'\b(\d+(?:\.\d+)?)(\w+)?\b').search(b)

    if not search:
        return

    if search.group(2) is None:
        return float(search.group(1))

    exp = predecs.index(search.group(2).lower())

    if exp > 0:
        return float(search.group(1))*10**(3*exp)

    exp = prebins.index(search.group(2).lower())

    if exp > 0:
        return float(search.group(1))*2**(10*exp)

def format_memory(b):
    predecs = ['B', 'kB',  'MB',  'GB',  'TB' ]

    for i in range(len(predecs)):
        if b <= 10**3:
            return f'{b:6.2f}{predecs[i]}'

        b /= 10**3

    return f'{b*10**3:.2f}{predecs[-1]}'
#endregion

#region util
def levenshtein(s1, s2):
    m, n = len(s1), len(s2)
    dp = [[0 for x in range(n+1)] for x in range(m+1)] 
  
    for i in range(m+1): 
        for j in range(n+1): 
  
            if i == 0: 
                dp[i][j] = j
            elif j == 0: 
                dp[i][j] = i
            elif s1[i-1] == s2[j-1]: 
                dp[i][j] = dp[i-1][j-1] 
            else: 
                dp[i][j] = 1 + min(dp[i][j-1],
                                   dp[i-1][j],
                                   dp[i-1][j-1])
  
    return dp[m][n]
#endregion

#region runners
def sandbox(cmd, stdin=None, memory_limit=None, time_limit=None):
    timer = '/usr/bin/time' if platform.system() == 'Linux' \
                            else '/usr/local/bin/gtime'

    with tempfile.NamedTemporaryFile('w+') as timefile:
        box = ''

        if memory_limit is not None:
            if isinstance(memory_limit, str):
                memory_limit = parse_memory(memory_limit)

            memory_limit = int(2*memory_limit/10**3)+1

            box += f'ulimit -v {memory_limit}; '
            box += f'ulimit -m {memory_limit}; '
        if time_limit is not None:
            if isinstance(time_limit, str):
                time_limit = parse_time(time_limit)

            time_limit = int(2*time_limit)+1

            box += f'ulimit -t {time_limit}; '
            box += f'timeout -k {time_limit} {time_limit} '

        box += f'sh -c "{escape(cmd)}" 2>&1'

        sand = [timer, '-o', timefile.name, '-f', '%e %M %x',
                'sh', '-c', box ]

        read, write = os.pipe()
        if stdin:
            os.write(write, stdin.encode('utf-8'))
        os.close(write)

        res = subprocess.run(sand, stdin=read, stdout=subprocess.PIPE)

        output  = res.stdout.decode('utf-8')
        elapsed, mpeek, status = timefile.read().strip().split()[-3:]

        elapsed = parse_time(elapsed)
        mpeek   = parse_memory(f'{mpeek}kB')
        status  = int(status)

    return output, elapsed, mpeek, status

def colordiff(width, output, expected, params):
    with tempfile.NamedTemporaryFile('w+') as outputfile:
        with tempfile.NamedTemporaryFile('w+') as expectedfile:
            outputfile.write(output)
            outputfile.flush()
            expectedfile.write(expected)
            expectedfile.flush()

            pipe = '| colordiff' if not sandbox('which colordiff')[3] else ''

            return sandbox(f'diff {params} --width={width} '
                           f'{outputfile.name} '
                           f'{expectedfile.name} '
                           f'{pipe}')[0]
#endregion

def main(verbose=False):
    """# MAC0121 EP1's unit test."""

    #region startup
    start = time.time()

    if sys.stdout.isatty():
        red     = '\033[31m'
        green   = '\033[32m'
        yellow  = '\033[33m'
        bold    = '\033[1m'
        default = '\033[0m'
        erase   = '\033[0K\r'
    else:
        red     = 'NO:'
        green   = 'OK:'
        yellow  = 'WA:'
        bold    = ''
        default = ''
        erase   = ''


    print(f'{bold}MAC0121 EP1\'s unit test.{default}\n')

    try:
        __, turtledorm, standard, nusp, email = sys.argv
    except:
        raise ValueError('Usage: python3.6 ./ep.c ./gabarito NUSP EMAIL')

    def _bool(expr):
        return green if expr else red

    def _bound(pred, true):
        if pred <= 1.01*(true+0.01):
            return green
        elif pred <= 2*(true+0.01):
            return yellow
        return red
    #endregion

    #region compile
    output, elapsed, mpeek, status = sandbox(f'make {os.path.splitext(turtledorm)[0]}')

    if status:
        code = red
    elif 'warning' in output:
        code = yellow
    else:
        code = green

    print(f'{code}COMPILE{default} {format_time(elapsed)} {format_memory(mpeek)}')
    print(output)

    print()
    #endregion

    #region read
    print(f'{bold}READ{default}',
          f'{os.path.basename(standard):>29s}',
          f'{os.path.basename(turtledorm):>19s}',
          f'  diff')

    for i, testpath in enumerate(sorted(glob('./testes/*.txt')), 1):
        stdin = f'l\n{testpath}\nd\n\n'

        print(f' {i: 3d}', f' {os.path.basename(testpath):>6s}', end='')
        
        std = sandbox(f'{os.path.join(".", standard)}', 
                      stdin=stdin)

        print(' |',
              ' ', f'{std[3]: 3d}', 
              ' ', format_time(std[1]), 
              ' ', format_memory(std[2]), 
              sep='', end='')

        res = sandbox(f'{os.path.splitext(turtledorm)[0]}.bin', 
                      stdin=stdin, memory_limit=10*std[2], time_limit=10*std[1])

        print(' |',
              ' ', _bool(std[3] == res[3]), std[3], default,
              ' ', _bound(res[1], std[1]), format_time(res[1]),   default,
              ' ', _bound(res[2], std[2]), format_memory(res[2]), default, 
              sep='', end='')

        diff = colordiff(160, res[0], std[0], '-Z -b')

        print(' | ', _bool(diff.strip() == ''), levenshtein(res[0], std[0]), default, 
              sep='')
        if not diff.strip() == '':
            print(colordiff(160, res[0], std[0], '-Z -b -y'))

    print()
    #endregion

    #region seed
    print(f'{bold}SEED{default}',
          f'{os.path.basename(standard):>29s}',
          f'{os.path.basename(turtledorm):>19s}',
          f'{os.path.basename(turtledorm):>19s}',
          f'  diff')

    random.seed(0)
    turns = 20

    for i in range(1, turns+1):
        rows = random.randint(1, 128)
        cols = random.randint(1, 128//rows)
        lvl  = 'fmd'[random.randint(0, 2)]

        stdin = f's\n{rows} {cols}\n{random.randrange(2**16-1)}\n{lvl}\nd\n\n'

        print(f' {i: 3d} {rows: >3d}x{cols: <3d}',  sep='', end='')
        
        std = sandbox(f'{os.path.join(".", standard)}', 
                      stdin=stdin)

        print(' |',
              ' ', f'{std[3]: 3d}', 
              ' ', format_time(std[1]), 
              ' ', format_memory(std[2]), 
              sep='', end='')

        res1 = sandbox(f'{os.path.splitext(turtledorm)[0]}.bin', 
                       stdin=stdin, memory_limit=10*std[2], time_limit=10*std[1])
        print(' |',
              ' ', _bool(std[3] == res1[3]), std[3], default,
              ' ', _bound(res1[1], std[1]), format_time(res1[1]),   default,
              ' ', _bound(res1[2], std[2]), format_memory(res1[2]), default, 
              sep='', end='')

        res2 = sandbox(f'{os.path.splitext(turtledorm)[0]}.bin', 
                       stdin=stdin, memory_limit=10*std[2], time_limit=10*std[1])
        print(' |',
              ' ', _bool(std[3] == res2[3]), std[3], default,
              ' ', _bound(res2[1], std[1]), format_time(res2[1]),   default,
              ' ', _bound(res2[2], std[2]), format_memory(res2[2]), default, 
              sep='', end='')

        diff = colordiff(160, res1[0], res2[0], '-Z -b')

        print(' | ', _bool(diff.strip() == ''), levenshtein(res1[0], res2[0]), default, 
              sep='')
        if not diff.strip() == '':
            print(colordiff(160, res1[0], res2[0], '-Z -b -y'))

    print()
    #endregion

    #region play
    print(f'{bold}PLAY{default}',
          f'{os.path.basename(standard):>32s}',
          f'{os.path.basename(turtledorm):>19s}',
          f'  diff')

    random.seed(0)
    turns = 5


    for i, testpath in enumerate(sorted(glob('./testes/*.txt')), 1):
        with open(testpath) as testfile:
            rows, cols = map(int, testfile.readline().strip().split())

        stdin = f'l\n{testpath}\n'

        plays = random.randint(0, turns)
        for j in range(plays):
            stdin += f'{random.randint(1, rows)} {random.randint(1, cols)}\n'

        stdin += 'd\n\n'

        print(f' {i: 3d} {os.path.basename(testpath):>6s} {plays: 3d}', end='')
        
        std = sandbox(f'{os.path.join(".", standard)}', 
                      stdin=stdin)

        print(' |',
              ' ', f'{std[3]: 3d}', 
              ' ', format_time(std[1]), 
              ' ', format_memory(std[2]), 
              sep='', end='')

        res = sandbox(f'{os.path.splitext(turtledorm)[0]}.bin', 
                      stdin=stdin, memory_limit=10*std[2], time_limit=10*std[1])

        print(' |',
              ' ', _bool(std[3] == res[3]), std[3], default,
              ' ', _bound(res[1], std[1]), format_time(res[1]),   default,
              ' ', _bound(res[2], std[2]), format_memory(res[2]), default, 
              sep='', end='')

        diff = colordiff(160, res[0], std[0], '-Z -b')

        print(' | ', _bool(diff.strip() == ''), levenshtein(res[0], std[0]), default, 
              sep='')
        if not diff.strip() == '':
            print(colordiff(160, res[0], std[0], '-Z -b -y'))

    print()
    #endregion

    #region solve
    print(f'{bold}SOLVE{default}',
          f'{os.path.basename(standard):>27s}',
          f'{os.path.basename(turtledorm):>19s}',
          f'  diff')


    for i, testpath in enumerate(sorted(glob('./testes/*.txt')), 1):
        with open(testpath) as testfile:
            rows, cols = map(int, testfile.readline().strip().split())

        stdin = f'l\n{testpath}\na\nd\n\n'

        print(f' {i: 3d} {os.path.basename(testpath):>6s}', end='')
        
        std = sandbox(f'{os.path.join(".", standard)}', 
                      stdin=stdin)

        print(' |',
              ' ', f'{std[3]: 3d}', 
              ' ', format_time(std[1]), 
              ' ', format_memory(std[2]), 
              sep='', end='')

        res = sandbox(f'{os.path.splitext(turtledorm)[0]}.bin', 
                      stdin=stdin, memory_limit=10*std[2], time_limit=10*std[1])

        print(' |',
              ' ', _bool(std[3] == res[3]), std[3], default,
              ' ', _bound(res[1], std[1]), format_time(res[1]),   default,
              ' ', _bound(res[2], std[2]), format_memory(res[2]), default, 
              sep='', end='')

        diff = colordiff(160, res[0], std[0], '-Z -b')

        print(' | ', _bool(diff.strip() == ''), levenshtein(res[0], std[0]), default, 
              sep='')
        if not diff.strip() == '':
            print(colordiff(160, res[0], std[0], '-Z -b -y'))

    print()
    #endregion

    #region tar
    os.remove(f'{os.path.splitext(turtledorm)[0]}.bin')

    assert os.path.basename(turtledorm) == 'turtledorm.c'
    assert not os.path.exists(nusp)

    with open('turtledorm.c') as programfile:
        assert 'AO PREENCHER ESSE CABEÇALHO COM O MEU NOME E O MEU NÚMERO USP' \
               in programfile.read()

    os.system(f'mkdir {nusp}')

    os.system(f'cp turtledorm.c {nusp}/turtledorm.c')
    os.system(f'cp Makefile {nusp}/Makefile')
    os.system(f'echo {email} > {nusp}/email.txt')
    os.system(f'echo {nusp} > {nusp}/nusp.txt')

    os.system(f'tar -cvf {nusp}.tar {nusp}')
    os.system(f'rm -rf {nusp}')
    #endregion

    print(f"{bold}elapsed: {format_time(time.time()-start)}{default}")


if __name__ == '__main__':
    main(verbose=len(sys.argv)>1)
