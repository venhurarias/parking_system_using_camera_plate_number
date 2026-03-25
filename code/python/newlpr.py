import requests

def recognize_license_plate(image_path):
    # Replace with your actual OpenALPR API key
    api_key = "your_openalpr_api_key"
    
    # Read the image file
    with open(image_path, 'rb') as image_file:
        image_data = image_file.read()

    # Set the API endpoint and parameters
    url = 'https://api.openalpr.com/v2/recognize'
    params = {
        'recognize_vehicle': 1,
        'country': 'us',
        'secret_key': api_key,
    }

    # Send the request to OpenALPR API
    response = requests.post(url, files={'image': image_data}, params=params)
    result = response.json()

    # Print the results
    print("Processing Time:", result['processing_time'])
    print("Results:")
    for plate in result['results']:
        print("Plate:", plate['plate'])
        print("Confidence:", plate['confidence'])
        print("Region:", plate['region'])
        print("Vehicle Type:", plate['vehicle']['type'])
        print("----")

# Example usage
image_path = 'path_to_your_license_plate_image.jpg'
recognize_license_plate(image_path)
