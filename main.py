from fastapi import FastAPI
from models import Item

app = FastAPI()

items = []

@app.get("/")
def read_root():
    return {"status": "ok", "message": "FastAPI is running"}

@app.post("/items") 
def create_item(item: Item): 
    items.append(item) 
    return {"status": "created", "item": item}

@app.get("/items") 
def get_items(): 
    return items

@app.put("/items/{index}")
def update_item(index: int, item: dict):
    items[index] = item
    return {"status": "updated", "item": item}

@app.delete("/items/{index}")
def delete_item(index: int):
    deleted_item = items.pop(index)
    return {"status": "deleted", "item": deleted_item}