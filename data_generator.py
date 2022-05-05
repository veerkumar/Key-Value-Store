import uuid
import random
import string

data_size = 1000000

def main():
	f = open("testdata_1M.data", "a")
	for i  in range(data_size):
		letters = string.ascii_lowercase
		letters_digit = string.digits
		value = (''.join(random.choice(letters) for i in range(int(''.join(random.choice(letters_digit) for i in range(2))))))
		if value == "":
			value = "null" 
		f.write(str(uuid.uuid4()) + " " + value + "\n")
	f.close()

if __name__ == '__main__':
	main()