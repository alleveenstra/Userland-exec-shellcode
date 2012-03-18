all: clean
	gcc -g -Wall libgule.c gule-client.c -o gule-client
	gcc -g -Wall gule-shellcode.c -o gule-shellcode
	gcc -g -Wall -static test.c -o test-s
	@strip test-s
	@echo "Build succesfully!"

clean:
	@touch gule-client gule-shellcode test-s
	@rm gule-client gule-shellcode test-s
