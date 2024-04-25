import serial
import sys
import os


def write_data_to_file(data, filename: str = "../data/data.txt") -> None:
    if os.path.exists(filename):
        os.remove(filename)
    with open(filename, "a") as file:
        num_columns = len(data)
        column_length = len(data[0])
        for line in range(column_length):
            line_of_data = ""
            for column in range(num_columns):
                val = data[column][line]
                line_of_data += str(val)
                if column != num_columns - 1:
                    line_of_data += ","
            line_of_data += "\n"
            file.writelines(line_of_data)


def parse_data(values_to_parse: list) -> tuple:
    x_vals = []
    y_vals = []
    for line in values_to_parse:
        split_line = line.split(",")
        x_val, y_val = split_line
        x_vals.append(float(x_val))
        y_vals.append(float(y_val))
    return (x_vals, y_vals)


def read_data() -> tuple:
    values_for_parsing = []
    count = 0
    while True:
        try:
            line = ser.readline()
            read_line = line.decode("utf-8")
            if count == 1000:
                # just as a check to see what is being read in
                print(read_line)
                count = 0

            if len(read_line) < 5:
                return parse_data(values_for_parsing)

            values_for_parsing.append(read_line)
            count += 1
        except:
            print("Bad read line: ", line.decode("utf-8"))
            ser.close()
            sys.exit(0)


if __name__ == "__main__":
    serial_port = "/dev/cu.usbmodem51015401"
    baud_rate = 115200
    ser = serial.Serial(
        serial_port,
        baud_rate,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=10,
    )

    while not ser.in_waiting:
        continue

    data = read_data()
    write_data_to_file(data)
