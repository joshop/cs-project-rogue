import random
import itertools
import sys

random.seed(int(open("seed").read()))
def _AStar(start, goal):
    def heuristic(a, b):
        ax, ay = a
        bx, by = b
        return abs(ax - bx) + abs(ay - by)

    def reconstructPath(n):
        if n == start:
            return [n]
        return reconstructPath(cameFrom[n]) + [n]

    def neighbors(n):
        x, y = n
        return (x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)

    closed = set()
    open = set()
    open.add(start)
    cameFrom = {}
    gScore = {start: 0}
    fScore = {start: heuristic(start, goal)}

    while open:
        current = None
        for i in open:
            if current is None or fScore[i] < fScore[current]:
                current = i

        if current == goal:
            return reconstructPath(goal)

        open.remove(current)
        closed.add(current)

        for neighbor in neighbors(current):
            if neighbor in closed:
                continue
            g = gScore[current] + 1

            if neighbor not in open or g < gScore[neighbor]:
                cameFrom[neighbor] = current
                gScore[neighbor] = g
                fScore[neighbor] = gScore[neighbor] + heuristic(neighbor, goal)
                if neighbor not in open:
                    open.add(neighbor)
    return ()


def generate(cellsX, cellsY, cellSize=5):
    # 1. Divide the map into a grid of evenly sized cells.

    class Cell(object):
        def __init__(self, x, y, id):
            self.x = x
            self.y = y
            self.id = id
            self.connected = False
            self.connectedTo = []
            self.room = None

        def connect(self, other):
            self.connectedTo.append(other)
            other.connectedTo.append(self)
            self.connected = True
            other.connected = True
    import os
    os.system("clear")
    print("Generating dungeon: 0/7")
    cells = {}
    for y in range(cellsY):
        for x in range(cellsX):
            c = Cell(x, y, len(cells))
            cells[(c.x, c.y)] = c
    os.system("clear")
    print("Generating dungeon: 1/7")
    # 2. Pick a random cell as the current cell and mark it as connected.
    current = lastCell = firstCell = random.choice(list(cells.values()))
    current.connected = True
    os.system("clear")
    print("Generating dungeon: 2/7")
    # 3. While the current cell has unconnected neighbor cells:
    def getNeighborCells(cell):
        for x, y in ((-1, 0), (0, -1), (1, 0), (0, 1)):
            try:
                yield cells[(cell.x + x, cell.y + y)]
            except KeyError:
                continue

    while True:
        unconnected = [x for x in getNeighborCells(current) if not x.connected]
        if not unconnected:
            break
        
        # 3a. Conqnect to one of them.
        neighbor = random.choice(unconnected)
        current.connect(neighbor)

        # 3b. Make that cell the current cell.
        current = lastCell = neighbor
    os.system("clear")
    print("Generating dungeon: 3/7")
    # 4. While there are unconnected cells:
    while [x for x in list(cells.values()) if not x.connected]:
        # 4a. Pick a random connected cell with unconnected neighbors and connect to one of them.
        candidates = []
        for cell in [x for x in list(cells.values()) if x.connected]:
            neighbors = [x for x in getNeighborCells(cell) if not x.connected]
            if not neighbors:
                continue
            candidates.append((cell, neighbors))
        cell, neighbors = random.choice(candidates)
        cell.connect(random.choice(neighbors))
    os.system("clear")
    print("Generating dungeon: 4/7")
    # 5. Pick 0 or more pairs of adjacent cells that are not connected and connect them.
    extraConnections = random.randint((cellsX + cellsY) // 4, int((cellsX + cellsY) // 1.2))
    maxRetries = 10
    while extraConnections > 0 and maxRetries > 0:
        cell = random.choice(list(cells.values()))
        neighbor = random.choice(list(getNeighborCells(cell)))
        if cell in neighbor.connectedTo:
            maxRetries -= 1
            continue
        cell.connect(neighbor)
        extraConnections -= 1
    os.system("clear")
    print("Generating dungeon: 5/7")
    # 6. Within each cell, create a room of random shape
    rooms = []
    for cell in list(cells.values()):
        width = random.randint(3, cellSize - 2)
        height = random.randint(3, cellSize - 2)
        x = (cell.x * cellSize) + random.randint(1, cellSize - width - 1)
        y = (cell.y * cellSize) + random.randint(1, cellSize - height - 1)
        floorTiles = []
        for i in range(width):
            for j in range(height):
                floorTiles.append((x + i, y + j))
        cell.room = floorTiles
        rooms.append(floorTiles)
    os.system("clear")
    print("Generating dungeon: 6/7")
    # 7. For each connection between two cells:
    connections = {}
    for c in list(cells.values()):
        for other in c.connectedTo:
            connections[tuple(sorted((c.id, other.id)))] = (c.room, other.room)
    for a, b in list(connections.values()):
        # 7a. Create a random corridor between the rooms in each cell.
        start = random.choice(a)
        end = random.choice(b)

        corridor = []
        for tile in _AStar(start, end):
            if tile not in a and not tile in b:
                corridor.append(tile)
        rooms.append(corridor)
    os.system("clear")
    print("Generating dungeon: 7/7")
    print("Processing dungeon: 0/1")
    # 8. Place staircases in the cell picked in step 2 and the lest cell visited in step 3b.
    stairsUp = random.choice(firstCell.room)
    stairsDown = random.choice(lastCell.room)
    # create tiles
    tiles = {}
    tilesX = cellsX * cellSize
    tilesY = cellsY * cellSize
    for x in range(tilesX):
        for y in range(tilesY):
            tiles[(x, y)] = " "
    for xy in itertools.chain.from_iterable(rooms):
        tiles[xy] = "."

    # every tile adjacent to a floor is a wall
    def getNeighborTiles(xy):
        tx, ty = xy
        for x, y in ((-1, -1), (0, -1), (1, -1),
                     (-1, 0), (1, 0),
                     (-1, 1), (0, 1), (1, 1)):
            try:
                yield tiles[(tx + x, ty + y)]
            except KeyError:
                continue

    for xy, tile in tiles.items():
        if not tile == "." and "." in getNeighborTiles(xy):
            tiles[xy] = "#"
    tiles[stairsUp] = "<"
    tiles[stairsDown] = ">"
    os.system("clear")
    print("Generating dungeon: 7/7")
    print("Processing dungeon: 1/1")
    # some new code: lava!
    def pathFound(): # is there a path from the upstairs -> downstairs?
        #input("pathFound() called")
        pathTiles = {}
        for a in tiles.keys():
            if a == stairsUp: pathTiles[a] = 1
            elif a == stairsDown: pathTiles[a] = 2
            else: pathTiles[a] = 0
        for iter in range(100): # eh should be enough iterations for now
            #print("iteration no. %d" % iter)
            for ta in range(50):
                s = ""
                for tb in range(50):
                    s += ' <>'[pathTiles[(ta,tb)]]
                #print(s)
            #input("press enter...")
            for t in pathTiles.keys():
                adj = [pathTiles.get(z) for z in [(t[0],t[1]+1),(t[0],t[1]-1),(t[0]+1,t[1]),(t[0]-1,t[1])] if (str(tiles.get(z)) in '<.>"' or type(tiles.get(z)) == int)and pathTiles.get(z)]
                if 1 in adj and 2 in adj: return True
                elif 1 in adj: pathTiles[t] = 1
                elif 2 in adj: pathTiles[t] = 2
        return False
    # random rects of lava covering floor BUT must keep a path
    for numVeinAttempts in range(20):
        os.system("clear")
        print("Generating dungeon: 7/7")
        print("Processing dungeon: 1/1")
        print("Adding lava: "+str(numVeinAttempts)+"/20")
        veinLeft, veinTop = (random.randint(0,cellsX*cellSize),random.randint(0,cellsY*cellSize))
        veinWidth, veinHeight = (random.randint(0,10),random.randint(0,10))
        old = tiles.copy()
        for iterW in range(veinWidth):
            for iterH in range(veinHeight):
                if not tiles.get((veinLeft+iterW,veinTop+iterH)): continue
                if tiles[(veinLeft+iterW,veinTop+iterH)] == ".":
                    tiles[(veinLeft+iterW,veinTop+iterH)] = ":"
        if not pathFound():
            tiles = old.copy()
    assert pathFound()
    for numVeinAttempts in range(30):
        os.system("clear")
        print("Generating dungeon: 7/7")
        print("Processing dungeon: 1/1")
        print("Adding lava: 20/20")
        print("Adding plants: "+str(numVeinAttempts)+"/30")
        veinLeft, veinTop = (random.randint(0,cellsX*cellSize),random.randint(0,cellsY*cellSize))
        veinWidth, veinHeight = (random.randint(0,10),random.randint(0,10))
        veinPlant = random.randint(0,119)
        for iterW in range(veinWidth):
            for iterH in range(veinHeight):
                if not tiles.get((veinLeft+iterW,veinTop+iterH)): continue
                if tiles[(veinLeft+iterW,veinTop+iterH)] == ".":
                    tiles[(veinLeft+iterW,veinTop+iterH)] = veinPlant
    outf = open("dungeon.out",'w')
    os.system("clear")
    print("Generating dungeon: 7/7")
    print("Processing dungeon: 1/1")
    print("Adding lava: 20/20")
    print("Adding plants: 30/30")
    print("Writing file: 0/1")
    for y in range(tilesY):
        for x in range(tilesX):
            if tiles[(x, y)] == " ":
              tiles[(x, y)] = '.'
            if type(tiles[(x, y)]) == int:
                outf.write(str(6+tiles[(x, y)])+" ")
            else:
                outf.write(str(".##:<>\"".index(tiles[(x, y)]))+" ")
    assert pathFound()
    outf.close()
    os.system("clear")
    print("Generating dungeon: 7/7")
    print("Processing dungeon: 1/1")
    print("Adding lava: 20/20")
    print("Adding plants: 30/30")
    print("Writing file: 1/1")
    print("Done!")
    import time
    time.sleep(0.5)
    os.system("clear")
    return tiles


if __name__ == "__main__":
    generate(5, 5, 10)
