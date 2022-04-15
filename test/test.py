import csv
from time import time
from time import sleep

def test_reader():
    ts = time()

    with open('csvreader.csv', 'r') as f:
        reader = csv.reader(f)

        for line in reader:
            continue

    te = time()
    print(f'Python csv.reader parses {reader.line_num} lines '
          f'in {(te-ts)*1000:.0f} milliseconds')


def test_dictreader():
    # sleep for 3 seconds
    sleep(3)

    ts = time()

    with open('csvreader.csv', 'r') as f:
        reader = csv.DictReader(f)

        for line in reader:
            continue

    te = time()
    print(f'Python csv.DictReader parses {reader.line_num} lines '
          f'in {(te-ts)*1000:.0f} milliseconds')


if __name__ == '__main__':
    test_reader()
    test_dictreader()