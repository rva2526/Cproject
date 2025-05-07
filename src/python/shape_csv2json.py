import pandas as pd 
import matplotlib.pyplot as plt
import json

def save_json(all_shapes, filename="./data/shape_data.json"):
    """Save the coordinates of multiple shapes to a JSON file."""
    json_data = {"shapes": all_shapes}
    
    # Serializing json
    json_object = json.dumps(json_data, indent=4)
    
    # Writing to a JSON file
    with open(filename, "w") as outfile:
        outfile.write(json_object)

    print('json saved to: {}\n'.format(outfile.name))

shape = pd.read_csv('/home/jovyan/Cproject/data/shape_data/southwest_cone_lower.csv',usecols=[0,1],sep='[\\s,]',names=['Easting','Northing'],header=0,skiprows=[-1],engine='python')
print('\nFile head: \n',shape.head())
shape.drop(shape.tail(1).index,inplace=True) # drop last n rows


all_shapes = []  # List to hold all shape data

#assuming single shape
shape_data = {
    'name': f"shape 1",
    'x': shape.Easting.to_list(),
    'y': shape.Northing.to_list(),
    'magnetic_inclination': 55,
    'magnetic_declination': 0,
    'magnetic_intensity': 20,
    'top': 150,
    'bottom': 210
}

all_shapes.append(shape_data)

# Save all shapes data to JSON
save_json(all_shapes,filename='/home/jovyan/Cproject/data/shape_data/southwest_cone_lower.json')