e ancientChamberClient

c LoadMesh
d s m_filename ancientchamber.agl

# Values for the atmosphere of a chamber
c EnvironmentValues
d f m_fogColorR 0.27
d f m_fogColorG 0.29
d f m_fogColorB 0.35
d f m_ambientColorR 0.22
d f m_ambientColorG 0.11
d f m_ambientColorB 0.19
# The values specifying the closeness(in percentage range 0-1) 
# of the fog start and end in relation to the near- and far planes.
d f m_fogFarPlaneClosenessPercentage 0.3
d f m_fogNearPlaneClosenessPercentage 1.0
# Radius of the "fog-volume" covering the chamber
d f m_minRadius 900.0
d f m_maxRadius 1000.0

# Transform data on the client is going to be overrided by the packet that is sent from the server.
c Transform
d f m_translationX 0.0
d f m_translationY 0.0
d f m_translationZ 0.0

c LevelPieceRoot