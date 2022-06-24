import capivara as cp
data = cp.Dataframe({
     'City_Code': [1, 2, 3, 4, 5],
     'Latitude': [-34.58, -15.78, -33.45, 4.60, 10.48],
     'Longitude': [-58.66, -47.91, -70.66, -74.08, -66.86],
})


data['City_Code'] = [10, 20, 30, 5, 50]



print("\n\nsimple column tests:\n")
print(data['Latitude'])
print(data['Longitude'])
print(data['City_Code'])



print("\n\nxy column tests:\n")
data['Geometry'] = cp.points_from_xy(data['Latitude'], data['Longitude'])
data['newColumn'] = cp.points_from_xy(data['Latitude'], data['City_Code'])
data['testColumn'] = [(1,2), (2,3), (3,4), (4,5), (5,6)]
print(data['Geometry'])
print(data['newColumn'])
print(data['testColumn'])



print("\n\nlisting columnns tests:\n")
print(data.columns())



print("\n\nfind tests without indexing:\n")
print(data.find_by_value('Latitude', -34.58))
print(data.find_by_value('City_Code', 10))


print("\n\nfind tests with indexing:\n")
data.set_index('City_Code')
print(data.find_by_value('City_Code', 10))
print(data.find_by_value('City_Code', 30))

data['new'] = [12, 20, 30, 40, 60]
data.set_index('new')
print(data.find_by_value('new', 60))
print(data.find_by_value('new', 10.45))
print(data.find_by_value('City_Code', 10))



print("\n\nRange find tests with indexing:\n")
data.set_index('Longitude')
print(data.between('Longitude', -30, 20.5))
print(data.between('Longitude', -50, 20.5))
print(data.between('City_Code', 10, 20))

data.set_index('City_Code')
print(data.between('City_Code', 10, 20))



print("\n\ninsert test:\n")
print(data['newColumn'])
print(data['testColumn'])
print(data['City_Code'])
print(data.columns())
data.insert_line([10,20.5,20,(10,10),(30,5),(0,-10), 17])
print(data['newColumn'])
print(data['testColumn'])
print(data['City_Code'])



print("\n\nplot test:\n")
data.plot('Latitude', 'Longitude')
