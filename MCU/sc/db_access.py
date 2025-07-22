import pymongo
import pymongo.errors
from rich import print
# from pymongo.collection import ObjectId

# the service is on localhost; operations should be almost instantaneous.
def shortTimeout(func):
    def inner(*args):
        try:
            with pymongo.timeout(1):
                func(*args)
        except pymongo.errors.PyMongoError as e:
            if e.timeout:
                print(f"Is the db up?\n{e}")
    return inner

def sample():
    client = pymongo.MongoClient(host="localhost", port=27017)
    db = client.get_database("oyster_db")
    print(db.list_collection_names())
    result:dict|None = db.get_collection("system_levels").find_one()
    client.close()
    if result is not None:
        print(result)
        for x in result:
            print(x)

@shortTimeout
def main():
    sample()


if __name__ == "__main__":
    main()
