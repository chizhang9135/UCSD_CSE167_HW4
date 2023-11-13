import json
import copy
import os


def generate_interpolated_jsons(filename):
    with open(filename, 'r') as f:
        data = json.load(f)

    # get user desired offset
    OFFSET = float(input("Enter offset: "))
    max_steps = 0

    # Determine the maximum steps required for camera transformations
    camera_transform = data["camera"]["transform"][0]["lookat"]
    for key, value in camera_transform.items():
        if isinstance(value, list):
            for v in value:
                steps = abs(v) / OFFSET
                if steps > max_steps:
                    max_steps = steps

    max_steps = int(max_steps)  # Convert max_steps to an integer

    for step in range(1, max_steps + 1):
        new_data = copy.deepcopy(data)
        step_offset = OFFSET * step

        # Apply the offset to camera transformations
        camera_transform = new_data["camera"]["transform"][0]["lookat"]
        for key, value in camera_transform.items():
            if isinstance(value, list):
                for i, v in enumerate(value):
                    if v > 0:
                        camera_transform[key][i] = min(v, step_offset)
                    else:
                        camera_transform[key][i] = max(v, -step_offset)

        output_filename = filename.split('.')[0] + f'_{step}.json'
        with open(output_filename, 'w') as f:
            json.dump(new_data, f, indent=4)

        print(f"Generated: {output_filename}")


if __name__ == "__main__":
    # get user input file name under a directory
    print("Create a directory and put your json file in it")
    fileName = input("Enter file name as format dir/file.json: ")
    generate_interpolated_jsons(fileName)
    # remove the original file from previous step
    os.remove(fileName)
    print(f"Original file {fileName} removed.")
