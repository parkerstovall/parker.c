# make -f makefiles/RunTests.mk | grep ":FAIL:" && echo "Tests Failed" || echo "All Tests Passed"

make -f makefiles/RunTests.mk | grep ":FAIL:";

if [[ $? -eq 1 ]]; then
    echo "Tests Passed!";
else 
    echo "Tests Failed...";
fi