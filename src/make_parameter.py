import numpy as np
import matplotlib.pyplot as plt
import json
import os
import pandas as pd
from shapely.geometry import Polygon


def center_shape(df: pd.DataFrame) -> None:
    coords = list(map(tuple, np.asarray(df)))
    poly = Polygon(coords)
    center = poly.centroid

    df['E_centered'] = df.Easting - center.x
    df['N_centered'] = df.Northing - center.y
    print(df.head())

def plot_line_and_polygon(shape: pd.DataFrame) -> None:
    fig, ax = plt.subplots()
    # ax.plot(model.line.Easting,model.line.Northing,color='red',markersize=3,label='Cleaned Observed Data')
    ax.plot(shape.E_centered,shape.N_centered,linestyle='dashed',color='black',label='Magnetic Body')
    ax.scatter(shape.E_centered,shape.N_centered,color='blue',label='Verticies')
    ax.scatter(0,0,color='black',marker='*',s=50)
    ax.legend()
    plt.savefig('./shape.png')

def generate_coordinates(vertices_number:int, width:int|float, height:int|float, center_x:int=0, center_y:int=0) -> list[float]:
    """Generate coordinates for a polygon with user-defined parameters."""
    
    half_width = width / 2
    half_height = height / 2
    
    if vertices_number < 4:
        raise ValueError("Polygon requires at least 4 vertices.")
    
    if vertices_number == 4:
        # Generate a rectangle (or square) aligned with axes
        coordinates = [
            (center_x - half_width, center_y - half_height),  # Bottom-left
            (center_x + half_width, center_y - half_height),  # Bottom-right
            (center_x + half_width, center_y + half_height),  # Top-right
            (center_x - half_width, center_y + half_height),  # Top-left
        ]
   
    elif vertices_number > 4:
        # General polygon with n number of vertices
        angles = np.linspace(0, 2 * np.pi, vertices_number, endpoint=False)
        coordinates = [
            (center_x + (half_width * np.cos(angle)), center_y + (half_height * np.sin(angle)))
            for angle in angles
        ]
    
    return coordinates

def plot_shapes_together(all_shapes: list, savefile="./plots/all_shapes.png"):
    """Plot all shapes together on a single figure."""
    
    plt.figure(figsize=(6, 6))
    
    # Plot each shape
    for shape_data in all_shapes:
        coordinates = [(x, y) for x, y in zip(shape_data['x'], shape_data['y'])]
        
        # Extract x and y coordinates
        x, y = zip(*coordinates)
        
        # Close the shape by adding the first point at the end
        x = list(x) + [x[0]]
        y = list(y) + [y[0]]
        
        # Plot the shape
        plt.plot(x, y, marker='o', linestyle='-', markersize=8)
        plt.fill(x, y, 'skyblue', alpha=0.4)  # Fill shape with transparency
    
    # Add grid and axis lines
    plt.axhline(0, color='gray', linestyle='--', linewidth=0.5)
    plt.axvline(0, color='gray', linestyle='--', linewidth=0.5)
    plt.grid(True, linestyle='--', alpha=0.5)
    
    # Set limits
    plt.xlim(-50, 50)
    plt.ylim(-50, 50)
    
    # Save and show the figure
    plt.savefig(savefile)
    print('plot saved to: {}\n'.format(savefile))

    # plt.show()

def save_json(all_shapes, filename="./data/shape_data.json"):
    """Save the coordinates of multiple shapes to a JSON file."""
    json_data = {"shapes": all_shapes}
    
    # Serializing json
    json_object = json.dumps(json_data, indent=4)
    
    # Writing to a JSON file
    with open(filename, "w") as outfile:
        outfile.write(json_object)

    print('json saved to: {}\n'.format(outfile.name))

def main():

    # csv = input("Do you have a csv file with shape verticies?: (yY/nN)")

    # if csv == 'n' or csv == 'N':
    #     print("Welcome to the Multiple Shape Generator!")
        
    #     # Get user input for the number of shapes
    #     num_shapes = int(input("How many shapes would you like to generate? "))
        
    #     all_shapes = []  # List to hold all shape data

    #     # Loop to generate multiple shapes
    #     for i in range(num_shapes):
    #         print(f"\nGenerating shape {i + 1}...")
            
    #         # Get user input for each shape's parameters
    #         vertices_number = int(input("Enter the number of vertices (minimum 4): "))
    #         width = float(input("Enter the width of the shape: "))
    #         height = float(input("Enter the height of the shape: "))
    #         center_x = float(input("Enter the center x-coordinate: "))
    #         center_y = float(input("Enter the center y-coordinate: "))
            
    #         # Additional magnetic parameters
    #         magnetic_inclination = float(input("Enter the magnetic inclination (degrees): "))
    #         magnetic_declination = float(input("Enter the magnetic declination (degrees): "))
    #         magnetic_intensity = float(input("Enter the magnetic intensity (amps/m): "))
    #         top = float(input("Enter the top value (meters from surface, down is positive): "))
    #         bottom = float(input("Enter the bottom value (meters from surface, down is positive): "))
    #         print('\n')
            
    #         # Generate shape coordinates
    #         coordinates = generate_coordinates(vertices_number, width, height, center_x, center_y)
    #         # Add the coordinates and parameters to the all_shapes list
    #         shape_data = {
    #             'name': f"shape {i + 1}",
    #             'x': [coord[0] for coord in coordinates],
    #             'y': [coord[1] for coord in coordinates],
    #             'magnetic_inclination': magnetic_inclination,
    #             'magnetic_declination': magnetic_declination,
    #             'magnetic_intensity': magnetic_intensity,
    #             'top': top,
    #             'bottom': bottom
    #         }
    #         all_shapes.append(shape_data)
        
    #     # Plot all shapes together
    #     plot_shapes_together(all_shapes)

    #     # Save all shapes data to JSON
    #     save_json(all_shapes)
        
    #     print(f"\n{num_shapes} shapes generated, plotted, and saved.")
    
    # elif csv == 'Y' or csv == 'y':

        # filepath = input("Ok, input the csv file path: (assuming x,y are the first two columns) ")

        df = pd.read_csv('data/shape_data/anom_a_cone_1.csv',skiprows=1,usecols=[0,1],sep='[\\s,]',names=['Easting','Northing'],engine='python')
        print('\nFile head: \n',df.head())

        center_shape(df)
        plot_line_and_polygon(shape=df)
        print("\nShape centered and plotted")

        vertices_number = df.index.max()+1

        print("\nGenerate the rest of the paramaters needed: \n")
        magnetic_inclination = float(input("Enter the magnetic inclination (degrees): "))
        magnetic_declination = float(input("Enter the magnetic declination (degrees): "))
        magnetic_intensity = float(input("Enter the magnetic intensity (amps/m): "))
        top = float(input("Enter the top value (meters from surface, down is positive): "))
        bottom = float(input("Enter the bottom value (meters from surface, down is positive): "))
        print('\n')

        all_shapes = []  # List to hold all shape data

        #assuming single shape
        shape_data = {
            'name': f"shape 1",
            'x': df.E_centered.to_list(),
            'y': df.N_centered.to_list(),
            'magnetic_inclination': magnetic_inclination,
            'magnetic_declination': magnetic_declination,
            'magnetic_intensity': magnetic_intensity,
            'top': top,
            'bottom': bottom
        }

        all_shapes.append(shape_data)

        # Save all shapes data to JSON
        save_json(all_shapes,filename='./cone1_shape.json')
        
        print(f"\n shapes generated, plotted, and saved.")

    # else:
    #     print("invalid input, exiting...")
    #     exit(1)


if __name__ == "__main__":
    main()
