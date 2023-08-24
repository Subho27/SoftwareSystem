echo "Welcome to Employee Management System"

InsertEmployee() {

	echo -e "Employee Record - Insertion\n"
	read -p "Enter the name of the Employee - " ename
	read -p "Enter the ID of the Employee - " id
        while [[ ! $id =~ ^[A-Za-z][A-Za-z0-9_]+$ ]]
        do
                echo -e "Incorrect ID format. Please Enter again.\n"
                read -p "Enter the ID of the Employee - " id
        done
	read -p "Enter the salary of the Employee(in K) - " salary
	
	echo "$id -- $ename -- $salary K" >> employee_data.txt
	echo -e "\nEmployee Record for $ename created."	
}

DeleteEmployee() {
	echo -e "Employee Record - Deletion\n"
	read -p "Enter ID of Employee to delete - " id
	checkEmp=$(grep "$id" employee_data.txt)
	if [[ $checkEmp ]]
	then
		sed -i "/$checkEmp/d" employee_data.txt
		echo "Employee details deleted successfully."
	else
		echo "Not able to delete the Employee."
	fi
}

SearchEmployee() {
	echo -e "Employee Record - Search\n"
        read -p "Enter ID of Employee to search - " id
        checkEmp=$(grep "$id" employee_data.txt)
        if [[ $checkEmp ]]
        then
                echo "Employee details found for $id."
        else
                echo "Employee details not found."
        fi
}

DisplayEmployee() {
	echo -e "Employee Record - Display\n"
        read -p "Enter ID of Employee to display - " id
        checkEmp=$(grep "$id" employee_data.txt)
        if [[ $checkEmp ]]
        then
                echo "Employee details for $id."
		echo "$checkEmp"
        else
                echo "Employee details not found."
        fi
}

SortRecords() {
	cat employee_data.txt | sort
}

ListAllRecords() {
	if [ -f "employee_data.txt" ]
	then
		cat "employee_data.txt"
	else
		echo "Database not found."
	fi
}

while [ 1 ]
do
	echo -e "\nMenu"
	echo "1. Insert Employee Record."
	echo "2. Delete Employee Record."
	echo "3. Search Employee Record."
	echo "4. Display Employee Details."
	echo "5. Sort Records."
	echo "6. List All Records."
	echo "7. Exit."

	read -p "Enter your choice (1-7) : " choice

	case $choice in
		1)
			InsertEmployee;;
		2)
			DeleteEmployee;;
		3)
			SearchEmployee;;
		4)
			DisplayEmployee;;
		5)
			SortRecords;;
		6)
			ListAllRecords;;
		7)
			echo "Employee Management System Logged off..."
			exit;;
	esac

done


