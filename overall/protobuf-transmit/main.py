import bottle_pb2
import json
from google.protobuf.json_format import MessageToJson

my_bottle = bottle_pb2.Bottle(note='Ahoy!')

with open('my_bottle.pb', 'wb') as f:
    f.write(my_bottle.SerializeToString())

with open("file.json", "w") as fjs:
    fjs.write(MessageToJson(my_bottle))

with open('my_bottle.pb', 'rb') as f:
    new_bottle = my_bottle.Bottle().FromString(f.read())
