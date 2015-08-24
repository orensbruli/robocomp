/*
 * 
 *    Copyright (C) 2015 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */
 

#include "agmInner.h"
#include <innermodel/innermodel.h>

AgmInner::AgmInner()
{
	worldModel = AGMModel::SPtr(new AGMModel());
	worldModel->name = "worldModel"; 
	innerModel = new InnerModel();
}

void AgmInner::setWorld(AGMModel::SPtr model)
{
	worldModel = model;
	//idea...
	///innerModel= extractInnerModel();
}
AGMModel::SPtr AgmInner::getWorld()
{
	return worldModel;
}


AgmInner::~AgmInner()
{

}


///functions
/**
 * @brief Search the name of the innermodel node,(the name is the unique key for innerModel ), inside de AGM Model. The innermodel id is stored in the attribute "name" of each symbol. 
 * It is found, return the id of the symbol, the unique key for AGMSymbols, otherwise returns -1.
 * 
 * @param n value of the attribute field name...
 * @return symbol ID, -1 if it is not found
 */
int AgmInner::findName(QString n)
{
	for (uint32_t i=0; i<worldModel->symbols.size(); ++i)
	{	
		if (worldModel->symbols[i]->attributes.find("imName") != worldModel->symbols[i]->attributes.end() )
		{
			try 
			{
				if (worldModel->symbols[i]->getAttribute("imName") == n.toStdString() )
				{
					qDebug()<<"findName: FOUND"<<n<<worldModel->symbols[i]->identifier;
					return worldModel->symbols[i]->identifier;
				}
			}
			catch (...)
			{
				std::cout<<"attribue name for symbol "<< worldModel->symbols[i]->symbolType <<" "<<i<<" not found \n";
			}
		}
	}	
// 	qDebug()<<"findName: NO ENCONTRADO"<<n<<-1;
	return -1;
}


/**
 * @brief Search the name of the innermodel node,(the name is the unique key for innerModel ), in the AGM Model parameter. The innermodel id is stored in the attribute "name" of each symbol. 
 * It is found, return the id of the symbol, the unique key for AGMSymbols, otherwise returns -1.
 * 
 * @param n value of the attribute field name...
 * @return symbol ID, -1 if it is not found
 */
int AgmInner::findName(const AGMModel::SPtr &m, QString n)
{
	for (uint32_t i=0; i<m->symbols.size(); ++i)
	{	
		if (m->symbols[i]->attributes.find("imName") != m->symbols[i]->attributes.end() )
		{
			if (m->symbols[i]->attributes["imName"] == n.toStdString() )
			{
// 				qDebug()<<"findName: FOUND"<<n<<m->symbols[i]->identifier;
				return m->symbols[i]->identifier;
			}
		}
	}	
// 	qDebug()<<"findName: NO ENCONTRADO"<<n<<-1;
	return -1;
}


/**
 * @brief 1 Find transform node reading his attribute "name" to get his corresponding symbol ID
 * 2 Go through rt edge starting from the agm symbol found before. Format Link a-RT->b
 * 3 Update the new InnerModel with the information stored in the edge
 * 
 * @param imNodeName InnerModelNode name to start the path
 * @return InnerModel* tree from InnerModelNode name
 */
InnerModel* AgmInner::extractInnerModel(QString imNodeName, bool ignoreMeshes)
{
	InnerModel *imNew = new InnerModel();
	int symbolID = findName(imNodeName);
	if (symbolID > -1)
		recorrer(imNew, symbolID, ignoreMeshes);
	return imNew;
}


/**
 * @brief Recorre el grafo por los enlaces RT desde el symbolID creando un innerModel equivalente
 * 
 * @param imNew Contiene el Innermodel equivalente generado
 * @param symbolID ID del symbolo punto de partida
 * @return void
 */
void AgmInner::recorrer(InnerModel* imNew, int& symbolID, bool ignoreMeshes)
{
	const AGMModelSymbol::SPtr &symbol = worldModel->getSymbol(symbolID);
	for (AGMModelSymbol::iterator edge_itr=symbol->edgesBegin(worldModel); edge_itr!=symbol->edgesEnd(worldModel); edge_itr++)
	{
		//std::cout<<(*edge_itr).toString(worldModel)<<"\n";
		//comprobamos el id del simbolo para evitar los arcos que le llegan y seguir solo los que salen del nodo
		if ((*edge_itr)->getLabel() == "RT" && (*edge_itr)->getSymbolPair().first==symbolID )
		{
			int second = (*edge_itr)->getSymbolPair().second;
			edgeToInnerModel((*edge_itr), imNew, ignoreMeshes);
			recorrer(imNew, second, ignoreMeshes);
		}
	}
}



/**
 * @brief ..transform the information contains in an AGM edge in two InnerModelNode, adding the information stored in the label RT 
 * to the father's transformation .
 * 
 * @param edge AGMModelEdge...
 * @param imNew ...
 * @return void
 */
void AgmInner::edgeToInnerModel(AGMModelEdge edge, InnerModel* imNew, bool ignoreMeshes) 
{
	InnerModelNode* nodeA = NULL;
	//InnerModelNode* nodeB = NULL;

	int first = edge->getSymbolPair().first;
	int second = edge->getSymbolPair().second;	

	const AGMModelSymbol::SPtr &symbolA = worldModel->getSymbol(first);
	const AGMModelSymbol::SPtr &symbolB = worldModel->getSymbol(second);

	QString nameA = QString::fromStdString(symbolA->getAttribute("imName"));
	QString nameB = QString::fromStdString(symbolB->getAttribute("imName"));
	
// 	qDebug()<<"insertar en new InnerModel "<<nameA<<"--"<< QString::fromStdString ( edge->getLabel() ) <<"-->"<<nameB;//<<tx<<ty<<tz<<rx<<ry<<rz;
// 	qDebug()<<"equivalente al enlace en AGM "<<QString::fromStdString (symbolA->toString())<<"--"<< QString::fromStdString ( edge->getLabel() ) <<"-->"<<QString::fromStdString (symbolB->toString());//<<tx<<ty<<tz<<rx<<ry<<rz;
	
	
	
	//entiendo que sino exite lo cuelgo del root, Estará vacio...
		
	//original
// 	nodeB = imNew->newTransform (nameB, "static",nodeA,tx,ty,tz,rx,ry,rz);	
// 	if (nodeB==NULL)
// 		qFatal("MAAAAAL edgeToInnerModel() nodeB == null ");	
// // 	original
// 	nodeA->addChild(nodeB);	
	
	float tx,ty,tz,rx,ry,rz;
	tx=ty=tz=rx=ry=rz=0.;
	tx = str2float(edge->attributes["tx"]);
	ty = str2float(edge->attributes["ty"]);
	tz = str2float(edge->attributes["tz"]);

	rx = str2float(edge->attributes["rx"]);
	ry = str2float(edge->attributes["ry"]);
	rz = str2float(edge->attributes["rz"]);
	
	//solo si es el root.
	nodeA=imNew->getNode(nameA);
	if (nodeA==NULL)
	{	
// 		qDebug()<<"node A null"<<nameA;
		insertSymbolToInnerModelNode(imNew,imNew->getRoot(), symbolA,tx,ty,tz,rx,ry,rz, ignoreMeshes);
		nodeA=imNew->getNode(nameA);
	}

	try
	{
		insertSymbolToInnerModelNode(imNew,nodeA, symbolB,tx,ty,tz,rx,ry,rz);		
	}
	catch(string e)
	{
		std::cout<<e<<"\n";
		qFatal("insertSymbolToInnerModelNode");
	}
}
void AgmInner::insertSymbolToInnerModelNode(InnerModel* imNew,InnerModelNode *parentNode, AGMModelSymbol::SPtr s, float tx, float ty, float tz, float rx, float ry, float rz, bool ignoreMeshes)
{
	
try 
	{
	QString nodeName = QString::fromStdString(s->getAttribute("imName"));
//  	std::cout<<"\nadding "<<nodeName.toStdString();
// 	std::cout<<" type "<<s->getAttribute("imType");
// 	std::cout<<" parent->id "<<parentNode->id.toStdString();
// 	std::cout<<" attrs.size() "<<s->attributes.size()<<"\n";
	
 
	if (s->getAttribute("imType")=="transform")
	{
		//std::cout<<"\t type: "<<s->getAttribute("imType") <<"\n";
		QString engine ="static";
		float mass =0.;
		try
		{
			 engine=QString::fromStdString(s->getAttribute("engine"));
		}
		catch (...)
		{
			//std::cout<<"\tattribute engine not found \n";			
		}		
		try
		{
			mass = str2float(s->getAttribute("mass"));
		 }
		catch (...)
		{
			//std::cout<<"\tattribute mass not found \n";			
		}		
		
		try
		{
			InnerModelTransform* tf=imNew->newTransform (nodeName, engine,parentNode,tx,ty,tz,rx,ry,rz, mass);
			parentNode->addChild(tf);
		}
		catch (...)
		{
			qDebug()<<"\tExiste transform "<<nodeName;
		}
	}
	else if (s->getAttribute("imType")=="plane")
	{
// // 		std::cout<<"\t type: "<<s->getAttribute("imType") <<"\n";
		float width=0.;
		float height=0.;
		float depth=0.;
		try
		{
			width= str2float(s->getAttribute("width")); 
		}
		catch (...)
		{
			
		}
		try
		{
			height= str2float(s->getAttribute("height"));
		}
		catch (...)
		{
			
		}
		try
		{
			depth=str2float(s->getAttribute("depth"));
		}
		catch (...)
		{
			
		}
		 
		float nx,ny,nz;
		nx=ny=nz=0.;
		try
		{
			nx= str2float(s->getAttribute("nx")); 
		}
		catch (...)
		{
			
		}
		try
		{
			ny= str2float(s->getAttribute("ny")); 
		}
		catch (...)
		{
			
		}
		try
		{
			nz= str2float(s->getAttribute("nz")); 
		}
		catch (...)
		{
			
		}
		
		float px,py,pz;		
		try
		{
			px= str2float(s->getAttribute("px")); 
		}
		catch (...)
		{
			
		}
		try
		{
			py= str2float(s->getAttribute("py")); 
		}
		catch (...)
		{
			
		}
		try
		{
			pz= str2float(s->getAttribute("pz")); 
		}
		catch (...)
		{
			
		}
				
		bool collidable = false;
		try
		{
			if (s->getAttribute("collidable")=="true")
				collidable = true;
		}
		catch (...)
		{
			
		}
		
		int repeat=0;
		try
		{
			repeat = str2int(s->getAttribute("repeat"));		
		}
		catch (...)
		{
			
		}
		
		QString texture="";
		try
		{
			texture =QString::fromStdString(s->getAttribute("texture"));
		}
		catch (...)
		{
			
		}
		
		try
		{
			InnerModelPlane *plane=imNew->newPlane (nodeName,parentNode, texture,width,height,depth, repeat,nx,ny,nz,px,py,pz,collidable);
			parentNode->addChild(plane);
		}
		catch (...)
		{
			qDebug()<<"\tExiste plane"<<nodeName;
		}

	}
	else if (s->getAttribute("imType")=="mesh")
	{
		if (ignoreMeshes) return;
// 		std::cout<<"\t type: "<<s->getAttribute("imType") <<"\n";
		QString meshPath="";
		try
		{
			meshPath = QString::fromStdString(s->getAttribute("path"));
		}
		catch (...)
		{
			qDebug()<<"Warnning!!!! mesh without path";
			throw;
		}
		
		float scalex,scaley,scalez;
		scalex=scaley=scalez=0.;
		
		try
		{
			scalex= str2float(s->getAttribute("scalex")); 
		}
		catch (...)
		{
			
		}
		try
		{
			scaley=str2float(s->getAttribute("scaley"));
		}
		catch (...)
		{
			
		}
		try
		{
			scalez=str2float(s->getAttribute("scalez"));
		}
		catch (...)
		{
			
		}
		
		bool collidable = false;
		try
		{
			if (s->getAttribute("collidable")=="true")
				collidable = true;
		}
		catch (...)
		{
			
		}
		
		//enum RenderingModes { NormalRendering=0, WireframeRendering=1};
		int render;
		render = InnerModelMesh::NormalRendering;
		try
		{
			if (s->getAttribute("render")=="WireframeRendering")
				render = InnerModelMesh::WireframeRendering;
		}
		catch (...)
		{
			
		}
			
		try
		{
			InnerModelMesh *mesh=imNew->newMesh(nodeName,parentNode,meshPath,scalex,scaley,scalez,render,tx,ty,tz,rx,ry,rz, collidable);
			parentNode->addChild(mesh);
		}
		catch (...)
		{
			qDebug()<<"\tExiste mesh"<<nodeName;
		}
	}
	else if (s->getAttribute("imType")=="rgbd")
	{
// 		std::cout<<"\t type: "<<s->getAttribute("imType") <<"\n";
		
		int port=0;	
		try
		{
			 port =str2int(s->getAttribute("port"));
		}
		catch (...)	{}

		float noise, focal, height, width;
		
		noise=0.;
		try
		{
			 noise = str2float(s->getAttribute("noise"));
		}
		catch (...)	{}
		focal=0.;
		try
		{
			 focal = str2float(s->getAttribute("focal"));
		}
		catch (...)	{}
		
		height=0.;
		try
		{
			 height = str2float(s->getAttribute("height"));
		}
		catch (...)	{}
		width=0.;
		try
		{
			 width = str2float(s->getAttribute("width"));
		}
		catch (...)	{}
		
		QString ifconfig="";		
		try
		{
			 ifconfig=QString::fromStdString(s->getAttribute("ifconfig"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}	
		
		try
		{
			InnerModelRGBD *rgbd=imNew->newRGBD(nodeName,parentNode,width,height,focal,noise,port,ifconfig);
			parentNode->addChild(rgbd);
		}
		catch (...)
		{
// 			qDebug()<<"\trgbd error"<<nodeName;
		}
	}
	else if (s->getAttribute("imType")=="camera")
	{
// 		std::cout<<"\t type: "<<s->getAttribute("imType") <<"\n";
				
		float focal, height, width;		
		focal=0.;
		try
		{
			 focal = str2float(s->getAttribute("focal"));
		}
		catch (...)	{}
		
		height=0.;
		try
		{
			 height = str2float(s->getAttribute("height"));
		}
		catch (...)	{}
		width=0.;
		try
		{
			 width = str2float(s->getAttribute("width"));
		}
		catch (...)	{}			
		
		try
		{
			InnerModelCamera *c =imNew->newCamera(nodeName,parentNode,width,height,focal);
			parentNode->addChild(c);
		}
		catch (...)
		{
			qDebug()<<"\tExists InnerModelCamera"<<nodeName;
		}
	}
	else if (s->getAttribute("imType")=="omniRobot")
	{
		int port=0;
		float noise =0.; 		
		try
		{
			 port= str2int(s->getAttribute("port"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}		
		try
		{
			noise = str2float(s->getAttribute("noise"));
		 }
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}		
				
		bool collide = false;
		try
		{
			if (s->getAttribute("collide")=="true")
				collide = true;
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}
		
		try
		{
			InnerModelOmniRobot *omni = imNew->newOmniRobot(nodeName,dynamic_cast<InnerModelTransform*>(parentNode),tx,ty,tz,rx,ry,rz,port,noise,collide);		
			parentNode->addChild(omni);
		}
		catch (...)
		{
			qDebug()<<"\tExists omni"<<nodeName;
		}
	}
	else if (s->getAttribute("imType")=="laser")
	{
		float angle =M_PIl;
		try
		{
			 angle= str2float(s->getAttribute("angle"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}	
		QString ifconfig="";		
		try
		{
			 ifconfig=QString::fromStdString(s->getAttribute("ifconfig"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}	
		int max = 30000;
		try
		{
			 max= str2int(s->getAttribute("max"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}	
		
		int measures =360;		
		try
		{
			 measures= str2int(s->getAttribute("measures"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}	
		int min =0;
		try
		{
			 min= str2int(s->getAttribute("min"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}	
		int port=0;
		try
		{
			 port= str2int(s->getAttribute("port"));
		}
		catch (...)
		{
// 			std::cout<<"\tattribute not found \n";			
		}	
		
		try
		{
			InnerModelLaser *laser = innerModel->newLaser(nodeName,parentNode,port,min,max,angle,measures,ifconfig);		
			parentNode->addChild(laser);
		}
		catch (...)
		{
// 			qDebug()<<"\tExists Laser"<<nodeName;
		}
	}
	else if (s->getAttribute("imType")=="joint")
	{
// 		std::cout<<"\t type: "<<s->getAttribute("imType") <<"\n";
		//QString id_, float lx_, float ly_, float lz_, float hx_, float hy_, float hz_, float tx_, float ty_, float tz_, float rx_, ;
		//float ry_, float rz_, float min_, float max_, uint32_t port_, std::string axis_, float home_, 
		
		///ESTO NO SE USA EN NINGUN InnerModel.XML
		float lx =0.;// str2float(s->attributes["lx"] );
		float ly =0.;// str2float(s->attributes["ly"] );
		float lz =0.;// str2float(s->attributes["lz"] );
		float hx =0.;// str2float(s->attributes["hx"] );
		float hy =0.;// str2float(s->attributes["hy"] );
		float hz =0.;// str2float(s->attributes["hz"] );				
		try
		{
			lx= str2float(s->getAttribute("lx")); 
		}
		catch (...){}
		try
		{
			ly=str2float(s->getAttribute("ly"));
		}
		catch (...){}
		try
		{
			lz=str2float(s->getAttribute("lz"));
		}
		catch (...){}
		
		try
		{
			hx= str2float(s->getAttribute("hx")); 
		}
		catch (...){}
		try
		{
			hy=str2float(s->getAttribute("hy"));
		}
		catch (...){}
		try
		{
			hz=str2float(s->getAttribute("hz"));
		}
		catch (...){}
		
		int port =0;		
		try
		{
			port=str2int(s->getAttribute("port"));
		}
		catch (...){}
		
		float min = -std::numeric_limits<float>::max();
		try
		{
			min = str2float(s->getAttribute("min"));
		}
		catch (...){}
		
		float max = std::numeric_limits<float>::max();
		try
		{
			max = str2float(s->getAttribute("max"));
		}
		catch (...){}
		
		float home = 0.;
		try
		{
			home = str2float(s->getAttribute("home"));
		}
		catch (...){}
				
		
		string axis="z";
		try
		{
			axis=s->getAttribute("axis");
		}
		catch (...){}
		
		
		
		try
		{			
			InnerModelTransform * tf = dynamic_cast<InnerModelTransform *>(parentNode);						
			InnerModelJoint *joint = imNew->newJoint(nodeName,tf,lx,ly,lz,hx,hy,hz,tx,ty,tz,rx,ry,rz,min,max,port,axis,home);		
			tf->addChild(joint);
			
		}
		catch (...)
		{
			qDebug()<<"\tExists joint"<<nodeName<<"parent"<< parentNode->id;
		}

	}
	else
	{
		QStringList l;
		l<<"robot"<<"object"<<"roomSt"<<"robotSt"<<"world"<<"objectSt";
	
		//el tipo no está en innerModel
		if ( l.contains(QString::fromStdString(s->symbolType)) )
		{
			std::cout<<"\t AGM SYMBOL, id imNode "<<s->toString()<<" type transform " <<"\n";
			InnerModelTransform* tf = imNew->newTransform (QString::fromStdString(s->toString()), "static",parentNode);	
			parentNode->addChild(tf);
		}
		
		else 
		{
			string err;			
			err = "\nsymbol "+ s->toString() + " the type is unknown or not implemented yet\n";
			std::cout<<err;
			throw err;
		}
		
	}
 }
	catch (...)
	{
		std::cout<<"\ncouldn't find the attribute imType in "<< s->toString()<<"\n";
		std::cout<<"\ns->attributes[imName]<< "<<s->getAttribute("imName")<<"\n";
		exit(0);
	}
	
}

void AgmInner::checkLoop(int& symbolID, QList<int> &visited, string linkType, bool &loop)
{
	if (visited.contains(symbolID) )
	{
		loop=true;
		visited.append(symbolID);
		return;
	}
	else
		visited.append(symbolID);
	
	const AGMModelSymbol::SPtr &symbol = worldModel->getSymbol(symbolID);
	for (AGMModelSymbol::iterator edge_itr=symbol->edgesBegin(worldModel); edge_itr!=symbol->edgesEnd(worldModel); edge_itr++)
	{
		//std::cout<<(*edge_itr).toString(worldModel)<<"\n";
		//comprobamos el id del simbolo para evitar los arcos que le llegan y seguir solo los que salen del nodo
		if ((*edge_itr)->getLabel() == linkType && (*edge_itr)->getSymbolPair().first==symbolID )
		{
			int second = (*edge_itr)->getSymbolPair().second;
			qDebug()<<symbolID<<"--"<<QString::fromStdString(linkType)<<"-->"<<second;
			qDebug()<<"\tvisited"<<visited;									
			checkLoop(second,visited, linkType,loop);
		}
	}
	
}

QList<int> AgmInner::getLinkedID (int symbolID, string linkType)
{
	const AGMModelSymbol::SPtr &symbol = worldModel->getSymbol(symbolID);
	QList<int> l;
	for (AGMModelSymbol::iterator edge_itr=symbol->edgesBegin(worldModel); edge_itr!=symbol->edgesEnd(worldModel); edge_itr++)
	{
		//std::cout<<(*edge_itr).toString(worldModel)<<"\n";			
		//comprobamos el id del simbolo para evitar los arcos que le llegan y seguir solo los que salen del nodo
		if ((*edge_itr)->getLabel() == linkType && (*edge_itr)->getSymbolPair().first==symbolID )
		{
			
			l.append((*edge_itr)->getSymbolPair().second);
		}
	}
	return l;
}

void AgmInner::updateAgmWithInnerModel(InnerModel* im)
{
	/// Vector of the edges that the model holds.	
	std::cout << "worldModel->edges.size(): "<<worldModel->edges.size();

	///tal vez sería bueno recorrer primero innerModel con include_im y crear attributes name por cada symbolo, pq puede haberse insertado algun nodo nuevo.
	for (std::vector<AGMModelEdge>::iterator it = worldModel->edges.begin() ; it != worldModel->edges.end(); ++it)
	{
		std::cout << ' ' << (*it)->toString(worldModel);
		if ((*it)->getLabel()=="RT" )
		{
			string songName;
			
			//obtengo del symbol hijo el atribute name
			try{
				songName= (worldModel->getSymbol( (*it)->getSymbolPair().second) )->getAttribute("imName");
				std::cout <<"\t"<<songName<<"\n";
				try 
				{
					InnerModelTransform *node= im->getTransform (QString::fromStdString(songName));
					(*it)->setAttribute("tx",float2str( node->getTr().x() ));
					(*it)->setAttribute("ty",float2str( node->getTr().y() ));
					(*it)->setAttribute("tz",float2str( node->getTr().z() ));
					(*it)->setAttribute("rx",float2str( node->getRxValue()));
					(*it)->setAttribute("ry",float2str( node->getRyValue()));
					(*it)->setAttribute("rz",float2str( node->getRzValue()));
				}
				catch (...)
				{
					qDebug()<<"edge EXCEPTION couldn't find attribute";
				}
			}
			catch (...)
			{
				qDebug()<<"EXCEPTION,RT label connect to a symbol without imName";
				std::cout<<(worldModel->getSymbol( (*it)->getSymbolPair().second))->toString(true);
			}
		}
		std::cout << '\n';
	}
}

//AGMMisc::publishEdgeUpdate(edge,agmagenttopic_proxy);
void AgmInner::updateAgmWithInnerModelAndPublish(InnerModel* im, AGMAgentTopicPrx &agmagenttopic_proxy)
{
	/// Vector of the edges that the model holds.	
	std::cout << "worldModel->edges.size(): "<<worldModel->edges.size();

	///tal vez sería bueno recorrer primero innerModel con include_im y crear attributes name por cada symbolo, pq puede haberse insertado algun nodo nuevo.
	for (std::vector<AGMModelEdge>::iterator it = worldModel->edges.begin() ; it != worldModel->edges.end(); ++it)
	{
		std::cout << ' ' << (*it)->toString(worldModel)<<"\n";
		if ((*it)->getLabel()=="RT" )
		{
			string songName;
			
			//obtengo del symbol hijo el atribute name
			try{
				string type = (worldModel->getSymbol( (*it)->getSymbolPair().second) )->getAttribute("imType");
				if ( type =="mesh" or type =="plane" )					
					continue;
				songName= (worldModel->getSymbol( (*it)->getSymbolPair().second) )->getAttribute("imName");
				std::cout <<"\t"<<songName<<"\n";
				try 
				{
					InnerModelTransform *node= im->getTransform (QString::fromStdString(songName));
					(*it)->setAttribute("tx",float2str( node->getTr().x() ));
					(*it)->setAttribute("ty",float2str( node->getTr().y() ));
					(*it)->setAttribute("tz",float2str( node->getTr().z() ));
					(*it)->setAttribute("rx",float2str( node->getRxValue()));
					(*it)->setAttribute("ry",float2str( node->getRyValue()));
					(*it)->setAttribute("rz",float2str( node->getRzValue()));
					AGMMisc::publishEdgeUpdate((*it),agmagenttopic_proxy);
					usleep(10000);
				}
				catch (...)
				{
					std::cout << '\t' << (*it)->toString(worldModel);
					qDebug()<<"\tedge EXCEPTION couldn't find attribute";
				}
			}
			catch (...)
			{
				qDebug()<<"EXCEPTION,RT label connect to a symbol without imName";
				std::cout<<(worldModel->getSymbol( (*it)->getSymbolPair().second))->toString(true);
			}
		}
		std::cout << '\n';
	}
}

AGMModel::SPtr AgmInner::extractAGM()
{
	AGMModel::SPtr  agmModel = AGMModel::SPtr(new AGMModel(worldModel));
	QList<int>l;
	
	for (AGMModel::iterator symbol_itr=agmModel->begin(); symbol_itr!=agmModel->end(); symbol_itr++)
	{
		std::cout<<" delete? "<<(*symbol_itr)->toString()<<"\n";
		bool removeSymbol = true;
		for (AGMModelSymbol::iterator  edge_itr=symbol_itr->edgesBegin(agmModel); edge_itr!=symbol_itr->edgesEnd(agmModel); edge_itr++)
		{
			std::cout<<"\t"<<(*edge_itr).toString(agmModel);
			//comprobamos el id del simbolo para evitar los arcos que le llegan y seguir solo los que salen del nodo
			if ((*edge_itr)->getLabel() != "RT" )//&& ( (*edge_itr)->symbolPair.first==(*symbol_itr)->identifier) )
			{				
				removeSymbol=false;				
				break;
			}
		}
		if (removeSymbol)
		{	
			std::cout<<"\t yes delete "<<(*symbol_itr)->toString()<<" ";
			//std::cout<<agmModel->removeSymbol((*symbol_itr))<<"\n";
			l.append((*symbol_itr)->identifier);
		}
		std::cout<<"\n";
	}
	for (int i = 0; i < l.size(); ++i) 
	{
		agmModel->removeSymbol(l.at(i));
	}
	std::cout<<"agmModel->numberOfSymbols() "<<agmModel->numberOfSymbols()<<"\n";
	return agmModel;
}


/**
 * @brief Elimina del AGM en caliente, los nodos del innerModel original que no esten en el agm original. 
 * 
 * @param agmFilePath ...
 * @param imFilePath ...
 * @return AGMModel::SPtr limpio de innerModel (normalmente el propio del robot) pero con la info geometrica (RT) actualizadas para los symbolos del mundo "exterior" 
 */
AGMModel::SPtr  AgmInner::remove_ImOriginal(string agmFilePath, string imFilePath)
{
	AGMModel::SPtr  agmTmp = AGMModel::SPtr(new AGMModel());
	cout<<"agmFilePath: " <<agmFilePath<<"\n";
	
	
	AGMModelConverter::fromXMLToInternal(agmFilePath, agmTmp);	
	InnerModel *imTmp= new InnerModel (imFilePath); 
	
	std::cout<<agmTmp->numberOfSymbols()<<" "<<agmTmp->numberOfEdges()<<"\n";
	AGMModelPrinter::printWorld(agmTmp);
	imTmp->treePrint();

	//if imNode not in agmOriginal, remove the symbol associated to the node in the agmCaliente if exist
	foreach (QString n, imTmp->getIDKeys() )
	{
		qDebug()<<n;
		//findName in the original
		if (findName(agmTmp,n)==-1 )
		{
			int symbolID=findName(n);
			if (symbolID!=-1)
			{
				qDebug()<<"remove en el agmCaliente :"<<symbolID<<QString::fromStdString( worldModel->getSymbol(symbolID)->toString());
			}
		}
	}
	
	return agmTmp;
}

void  AgmInner::remove_Im( InnerModel *imTmp)
{
	//if imNode not in agmOriginal, remove the symbol associated to the node in the agmCaliente if exist
	imTmp->save("imTmp.xml");
	qDebug()<<imTmp->getIDKeys();
	foreach (QString n, imTmp->getIDKeys() )
	{
		qDebug()<<n;
		
		int symbolID=-1;
		symbolID=findName(n);
		if (symbolID!=-1)
		{
			qDebug()<<"remove en el agmCaliente :"<<symbolID<<QString::fromStdString( worldModel->getSymbol(symbolID)->toString());
			worldModel->removeSymbol(symbolID);
		}	
	}
}

/************************************************************************************************

					INCLUDE METHODS

************************************************************************************************/


/**
 * @brief Insert innermodel in AGM graph matching nodes from innerModel to their correspondent symbols. 
 * Given the InnerModelNode ID as key in the hash, it inserts under the AGM symbol (specified by its ID as value in the hash) the counterpart subgraph.
 * If there is any relationship, parents-->children in innermodel. It is created a edge between the corresponding AGMSymbols, if it the edge exist, it is added a new RT link.
 * 
 * @param matchNode hash innerModelNode symbolID. 
 * @return void
 */
void AgmInner::include_im(QHash<QString, int32_t>  match, InnerModel *im)
{
	qDebug()<<match;
	QHash<QString, int32_t>::const_iterator i = match.constBegin();
	
	QList<QString> lNode =match.keys();
	
	//comprobar que todos los nodes existen en im
	for (int i = 0; i < lNode.size(); ++i) 
	{
		InnerModelNode *node=im->getNode(lNode.at(i));
		if (node==NULL)
		{
			qDebug()<<"node"<<lNode.at(i)<<"doesn't exist";
			qDebug()<<lNode;
			qFatal("abort, not node");
		}
	}
	//comprobar que todos los symbolos existen en AGM y que tienen attribute imName, imType y todo los de innermodel
	QList<int32_t> lSymbols =match.values();
	for (int i = 0; i < lSymbols.size(); ++i) 
	{
		try
		{
			 
			AGMModelSymbol::SPtr symbol = worldModel->getSymbolByIdentifier(lSymbols.at(i));
// 			symbol->setAttribute("imName",match.key(lSymbols.at(i)).toStdString());
// 			symbol->setAttribute("imType",match.key(lSymbols.at(i)).toStdString());
			std::map<std::string, std::string> attributes = ImNodeToSymbol(im->getNode(lNode.at(i)));
			for (auto m : attributes)
			{
				symbol->setAttribute(m.first,m.second);				
			}			
			
			try
			{
				symbol->getAttribute("imName");
			}
			catch (...)			
			{
				std::cout<<"AGMSymbol "<< symbol->toString()<<" ";
				std::cout<<"must be the attribute imName, ADDING [imName , "<<match.key(lSymbols.at(i)).toStdString()<<" ]\n";
				symbol->setAttribute("imName",match.key(lSymbols.at(i)).toStdString());
				
			}			
			try
			{
				symbol->getAttribute("imType");
			}
			catch (...)			
			{
				std::cout<<"AGMSymbol "<< symbol->toString()<<" ";
				std::cout<<"must be the attribue imType, ADDING [imType , "<<match.key(lSymbols.at(i)).toStdString()<<" ]\n";
				symbol->setAttribute("imType",match.key(lSymbols.at(i)).toStdString());
			}
		}
		catch (AGMModelException e )	
		{
			std::cout<<e.what();
			qDebug()<<"symbol ID"<<lSymbols.at(i)<<"doesn't exist";
			qFatal("Abort, not symbol");
		}
	}
	
	//comprobar que todos los symbolos de los enlaces RT de agm tienen attribute name y todos los de innermodel
	for (std::vector<AGMModelEdge>::iterator it = worldModel->edges.begin() ; it != worldModel->edges.end(); ++it)
	{
		//std::cout << ' ' << (*it)->toString(worldModel);
		if ((*it)->getLabel()=="RT" )
		{			
			AGMModelSymbol::SPtr symbolA = worldModel->getSymbol((*it)->getSymbolPair().first);			
			try
			{
				symbolA->getAttribute("imName");
			}
			catch (...)			
			{
				std::cout<<"AGMSymbol "<< symbolA->toString()<<" ";
				std::cout<<"must be the attribute imName, ADDING [imName , "<<symbolA->toString()<<" ]\n";
				symbolA->setAttribute("imName",symbolA->toString());
				symbolA->setAttribute("imType","transform");
				
			}	
			AGMModelSymbol::SPtr symbolB = worldModel->getSymbol((*it)->getSymbolPair().second);
			try
			{
				symbolB->getAttribute("imName");
			}
			catch (...)			
			{
				std::cout<<"AGMSymbol "<< symbolB->toString()<<" ";
				std::cout<<"must be the attribute name, ADDING [imName , "<<symbolB->toString()<<" ]\n";
				symbolB->setAttribute("imName",symbolB->toString());
				symbolA->setAttribute("imType","transform");
			}	
		}
		//std::cout << '\n';
	}
	
// 	qDebug()<<"***** lNode"<< lNode;
// 	qDebug()<<"***** lSymbols"<<lSymbols;
	
	//SI existe una relacion en innermodel entre dos nodos inserto un enlace RT en sus correspondientes symbolos
	for (int i = 0; i < lNode.size(); ++i) 
	{		
		InnerModelNode *node=im->getNode(lNode.at(i));		
		for (int j = 0; j < lNode.size(); ++j) 
		{
			InnerModelNode *nodeSong=im->getNode(lNode.at(j));
			qDebug()<<"InnerModel link"<<node->id <<"--RT-->"<<nodeSong->id;	
			if ( node->children.contains(nodeSong) )
			{
				//si no existe ya esta relacion
				//edge 
				try
				{
					AGMModelEdge edge  = worldModel->getEdgeByIdentifiers(lSymbols.at(i), lSymbols.at(j), "RT");
										
				}
				catch (...)
				{
					qDebug()<<"crear en AGM, link"<<lSymbols.at(i)<<"--RT-->"<<lSymbols.at(j);	
					std::map<std::string, std::string> linkAttrs;
					linkAttrs.insert ( std::pair<std::string,std::string>("tx",float2str(0.0)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("ty",float2str(0.0)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("tz",float2str(0.0)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("rx",float2str(0.0)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("ry",float2str(0.0)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("rz",float2str(0.0)) );
					worldModel->addEdgeByIdentifiers(lSymbols.at(i),lSymbols.at(j),"RT",linkAttrs);
				}
			}
		}
	}
	
	//ya están chequeados lo que viene en match esta bien y en l
	while (i != match.constEnd()) 
	{
// 		qDebug() << i.key() << ":" << i.value() << endl;
		lNode.removeAll(i.key());		
// 		qDebug()<<"innerToAGM ( "<<i.key()<<i.value()<<lNode<<" ) ";
		int32_t sId=i.value();
		innerToAGM(im->getNode(i.key()),sId,lNode);
		++i;
	}
	

}


void AgmInner::innerToAGM(InnerModelNode* node, int &symbolID, QList<QString>  lNode)
{
	QList<InnerModelNode*>::iterator i;	
	int p=symbolID;
	
	for (i=node->children.begin(); i!=node->children.end(); i++)
	{
		if ( !lNode.contains((*i)->id) )
		{	
			//Search name (key) of the innerModel node in AGM
			int existingID = findName ( (*i)->id ) ;
			if ( existingID == -1 )
			{	
				qDebug()<<node->id<<"link"<<(*i)->id;
				//symbol
				std::map<std::string, std::string> attrs;
				attrs = ImNodeToSymbol((*i));	
				int32_t id =worldModel->getNewId();
				AGMModelSymbol::SPtr newSym = worldModel->newSymbol(id,attrs["imType"],attrs);
				
				//edge 
				std::map<std::string, std::string> linkAttrs;
				if (newSym->typeString()=="mesh")
				{
					InnerModelMesh* mesh = dynamic_cast<InnerModelMesh*>((*i));
					linkAttrs.insert ( std::pair<std::string,std::string>("tx",float2str(mesh->tx)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("ty",float2str(mesh->ty)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("tz",float2str(mesh->tz)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("rx",float2str(mesh->rx)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("ry",float2str(mesh->ry)) );
					linkAttrs.insert ( std::pair<std::string,std::string>("rz",float2str(mesh->rz)) );
				}
				else
				{
					linkAttrs.insert ( std::pair<std::string,std::string>("tx",float2str((*i)->getTr().x())) );
					linkAttrs.insert ( std::pair<std::string,std::string>("ty",float2str((*i)->getTr().y())) );
					linkAttrs.insert ( std::pair<std::string,std::string>("tz",float2str((*i)->getTr().z())) );
					linkAttrs.insert ( std::pair<std::string,std::string>("rx",float2str((*i)->getRxValue())));
					linkAttrs.insert ( std::pair<std::string,std::string>("ry",float2str((*i)->getRyValue())));
					linkAttrs.insert ( std::pair<std::string,std::string>("rz",float2str((*i)->getRzValue())));
				}
				
				worldModel->addEdgeByIdentifiers(p,id,"RT",linkAttrs);
				innerToAGM((*i),id,lNode);
			}
			else
				innerToAGM((*i),existingID,lNode);
		}
	}	
}

std::map<std::string, std::string> AgmInner::ImNodeToSymbol(InnerModelNode* node)
{
	
				
	qDebug()<<node->id;
	std::map<std::string, std::string> attrs;
	attrs.insert ( std::pair<std::string,std::string>("imName",node->id.toStdString()) );
	
	
	
	//TODO innerModelNode cast to the type and translate the name.
	// attribute "type" = mesh,plane,joint..
	string type;
	if (dynamic_cast<InnerModelJoint*>(node) != NULL)
	{
		InnerModelJoint *joint = dynamic_cast<InnerModelJoint*>(node);
		//QString id_, float lx_, float ly_, float lz_, float hx_, float hy_, float hz_, float tx_, float ty_, float tz_, float rx_, ;
		//float ry_, float rz_, float min_, float max_, uint32_t port_, std::string axis_, float home_, 
// 		attrs.insert ( std::pair<std::string,std::string>("lx",float2str(0.) ) );
// 		attrs.insert ( std::pair<std::string,std::string>("ly",float2str(0.) ) );
// 		attrs.insert ( std::pair<std::string,std::string>("lz",float2str(0.) ) );
// 		
// 		attrs.insert ( std::pair<std::string,std::string>("hx",float2str(joint->backhX) ) );
// 		attrs.insert ( std::pair<std::string,std::string>("hy",float2str(joint->backhY) ) );
// 		attrs.insert ( std::pair<std::string,std::string>("hz",float2str(joint->backhZ) ) );
		
		
		attrs.insert ( std::pair<std::string,std::string>("min",float2str(joint->min) ) );
		attrs.insert ( std::pair<std::string,std::string>("max",float2str(joint->max) ) );
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(joint->port) ) );
		attrs.insert ( std::pair<std::string,std::string>("axis",joint->axis) );
		attrs.insert ( std::pair<std::string,std::string>("home",float2str(joint->home)) );
		//CAUTION no se si es neceario
		attrs.insert ( std::pair<std::string,std::string>("angle",float2str(joint->getAngle())) );
		
		type= "joint";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelPrismaticJoint*>(node) != NULL)
	{
		InnerModelPrismaticJoint *p = dynamic_cast<InnerModelPrismaticJoint*>(node);
// 	float value, offset;
// 	float min, max;
// 	float home;
// 	uint32_t port;
// 	std::string axis;
	
		attrs.insert ( std::pair<std::string,std::string>("value",float2str(p->value) ) );
		attrs.insert ( std::pair<std::string,std::string>("offset",float2str(p->offset) ) );
		attrs.insert ( std::pair<std::string,std::string>("min",float2str(p->min) ) );
		attrs.insert ( std::pair<std::string,std::string>("max",float2str(p->max) ) );
		attrs.insert ( std::pair<std::string,std::string>("home",float2str(p->home)) );
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(p->port)) );
		attrs.insert ( std::pair<std::string,std::string>("axis",p->axis) );
		
		type= "prismaticJoint";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	
	else if (dynamic_cast<InnerModelTouchSensor*>(node) != NULL)
	{
		InnerModelTouchSensor *touch = dynamic_cast<InnerModelTouchSensor*>(node);
// 		float nx, ny, nz;
		attrs.insert ( std::pair<std::string,std::string>("nx",float2str(touch->nx) ));
		attrs.insert ( std::pair<std::string,std::string>("ny",float2str(touch->ny)) );
		attrs.insert ( std::pair<std::string,std::string>("nz",float2str(touch->nz)) );
		
		//float min, max;// 	float value;
		attrs.insert ( std::pair<std::string,std::string>("min",float2str(touch->min) ));
		attrs.insert ( std::pair<std::string,std::string>("max",float2str(touch->max)) );
		attrs.insert ( std::pair<std::string,std::string>("value",float2str(touch->value)) );
	// 	uint32_t port;
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(touch->port)) );
		// QString stype;
		attrs.insert ( std::pair<std::string,std::string>("stype",touch->stype.toStdString()) );
		type = "touchSensor";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelDifferentialRobot*>(node) != NULL)
	{
		InnerModelDifferentialRobot *diff = dynamic_cast<InnerModelDifferentialRobot*>(node);
		//uint32_t port;
		//float noise;
		//bool collide;
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(diff->port)) );
		attrs.insert ( std::pair<std::string,std::string>("noise",float2str(diff->noise)) );
		
		string v="false";
		if (diff->collide)
			v="true";
		attrs.insert ( std::pair<std::string,std::string>("collide",v) );
		
		type = "differentialRobot";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelOmniRobot*>(node) != NULL)
	{
		InnerModelOmniRobot *omni = dynamic_cast<InnerModelOmniRobot*>(node);
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(omni->port)) );		
		attrs.insert ( std::pair<std::string,std::string>("noise",float2str(omni->noise)) );
		
		string v="false";
		if (omni->collide)
			v="true";
		attrs.insert ( std::pair<std::string,std::string>("collide",v) );
		type="omniRobot";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelPlane*>(node) != NULL)
	{
		//QString id, InnerModelNode *parent, QString texture, float width, float height, float depth, int repeat, 
		//float nx, float ny, float nz, float px, float py, float pz, bool collidable)

		InnerModelPlane* plane = dynamic_cast<InnerModelPlane*>(node);	
		
		attrs.insert ( std::pair<std::string,std::string>("width",float2str(plane->width) ));
		attrs.insert ( std::pair<std::string,std::string>("height",float2str(plane->height)) );
		attrs.insert ( std::pair<std::string,std::string>("depth",float2str(plane->depth)) );
		
		attrs.insert ( std::pair<std::string,std::string>("nx",float2str(plane->normal.x()) ));
		attrs.insert ( std::pair<std::string,std::string>("ny",float2str(plane->normal.y()) ));
		attrs.insert ( std::pair<std::string,std::string>("nz",float2str(plane->normal.z()) ));
		
		attrs.insert ( std::pair<std::string,std::string>("px",float2str(plane->point.x()) ));
		attrs.insert ( std::pair<std::string,std::string>("py",float2str(plane->point.y()) ));
		attrs.insert ( std::pair<std::string,std::string>("pz",float2str(plane->point.z()) ));
		
		string v="false";
		if (plane->collidable)
			v="true";
		attrs.insert ( std::pair<std::string,std::string>("collidable",v) );
		attrs.insert ( std::pair<std::string,std::string>("repeat",int2str(plane->repeat)) );
		attrs.insert ( std::pair<std::string,std::string>("texture",plane->texture.toStdString()) );
		type = "plane";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
		
	}
	else if (dynamic_cast<InnerModelRGBD*>(node) != NULL)
	{
		InnerModelRGBD* rgbd = dynamic_cast<InnerModelRGBD*>(node);		
		
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(rgbd->port)) );
		attrs.insert ( std::pair<std::string,std::string>("noise",float2str(rgbd->noise) ) );
		attrs.insert ( std::pair<std::string,std::string>("focal",float2str(rgbd->focal) ) );
		attrs.insert ( std::pair<std::string,std::string>("height",float2str(rgbd->height) ) );
		attrs.insert ( std::pair<std::string,std::string>("width",float2str(rgbd->width) ) );
		attrs.insert ( std::pair<std::string,std::string>("ifconfig",rgbd->ifconfig.toStdString()) );
		type ="rgbd";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelCamera*>(node) != NULL)
	{
		InnerModelCamera* cam = dynamic_cast<InnerModelCamera*>(node);		
		
		attrs.insert ( std::pair<std::string,std::string>("focal",float2str(cam->focal) ) );
		attrs.insert ( std::pair<std::string,std::string>("height",float2str(cam->height) ) );
		attrs.insert ( std::pair<std::string,std::string>("width",float2str(cam->width) ) );
		
		type = "camera";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelIMU*>(node) != NULL)
	{
		InnerModelIMU* imu = dynamic_cast<InnerModelIMU*>(node);		
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(imu->port)) );
		type = "imu";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelLaser*>(node) != NULL)
	{
		InnerModelLaser* laser = dynamic_cast<InnerModelLaser*>(node);
		//public:	uint32_t port;	uint32_t min, max;	float angle;	uint32_t measures;	QString ifconfig;
		
		attrs.insert ( std::pair<std::string,std::string>("port",int2str(laser->port)) );
		attrs.insert ( std::pair<std::string,std::string>("min",int2str(laser->min)) );
		attrs.insert ( std::pair<std::string,std::string>("max",int2str(laser->max)) );
		attrs.insert ( std::pair<std::string,std::string>("measures",int2str(laser->measures)) );
		attrs.insert ( std::pair<std::string,std::string>("angle",float2str(laser->angle)) );
		attrs.insert ( std::pair<std::string,std::string>("ifconfig",laser->ifconfig.toStdString()) );
		type = "laser";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelMesh*>(node) != NULL)
	{
		InnerModelMesh* mesh = dynamic_cast<InnerModelMesh*>(node);
		//InnerModelMesh *InnerModel::newMesh(QString id, InnerModelNode *parent, QString path, float scale, int render, float tx, float ty, float tz, float rx, float ry, float rz, bool collidable)

	     //return newMesh(id,parent,path,scale,scale,scale,render,tx,ty,tz,rx,ry,rz, collidable);
		attrs.insert ( std::pair<std::string,std::string>("path",mesh->meshPath.toStdString()) );
		attrs.insert ( std::pair<std::string,std::string>("scalex",float2str(mesh->scalex)) );
		attrs.insert ( std::pair<std::string,std::string>("scaley",float2str(mesh->scaley)) );
		attrs.insert ( std::pair<std::string,std::string>("scalez",float2str(mesh->scalez)) );
		string v="false";
		if (mesh->collidable)
			v="true";
		attrs.insert ( std::pair<std::string,std::string>("collidable",v) );
		
		//enum RenderingModes { NormalRendering=0, WireframeRendering=1};
		v="NormalRendering";
		if (mesh->render==1)
			v="WireframeRendering";
		attrs.insert ( std::pair<std::string,std::string>("render",v) );
		
		type = "mesh";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelPointCloud*>(node) != NULL)
	{
		//InnerModelPointCloud* pc = dynamic_cast<InnerModelPointCloud*>(node);		
		//attrs.insert ( std::pair<std::string,std::string>("generic",node->id.toStdString()) );
		type = "pointCloud";
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	else if (dynamic_cast<InnerModelTransform*>(node) != NULL)
	{
		InnerModelTransform *t = dynamic_cast<InnerModelTransform*>(node);
		///InnerModelTransform *newTransform(QString id, QString engine, InnerModelNode *parent, float tx=0, float ty=0, float tz=0, float rx=0, float ry=0, float rz=0, 
		//float mass=0);
		type="transform";
		attrs.insert ( std::pair<std::string,std::string>("engine",t->engine.toStdString()) );
		attrs.insert ( std::pair<std::string,std::string>("mass",float2str(t->mass)) );
		attrs.insert ( std::pair<std::string,std::string>("imType",type ) );
	}
	
	else
	{	
		string err;			
		err = "error: Type of node " + node->id.toStdString() + " is unknown.";
		throw err;
	}		
	return attrs;
}
void AgmInner::updateImNodeFromEdge(AGMModelEdge edge, InnerModel* inner)
{
	
		if (edge->getLabel()=="RT" )
		{
			string songName;
			//obtengo del symbol hijo el atribute name
			try{
				songName= (worldModel->getSymbol( edge->getSymbolPair().second) )->getAttribute("imName");
				string parentName= (worldModel->getSymbol( edge->getSymbolPair().first) )->getAttribute("imName");
				//std::cout <<"\t "<<parentName<<" "<<songName<<"\n";
				try 
				{
					
					float tx,ty,tz,rx,ry,rz;
					tx=str2float(edge->getAttribute("tx"));
					ty=str2float(edge->getAttribute("ty"));
					tz=str2float(edge->getAttribute("tz"));
					
					rx=str2float(edge->getAttribute("rx"));
					ry=str2float(edge->getAttribute("ry"));
					rz=str2float(edge->getAttribute("rz"));
					//qDebug() <<"tx,ty,tz,rx,ry,rz "<<tx<<ty<<tz<<rx<<ry<<rz;
					
					
					inner->updateTransformValues(QString::fromStdString(songName),tx,ty,tz,rx,ry,rz);
				}
				catch (...)
				{
					qDebug()<<"edge EXCEPTION couldn't find attribute";
				}
			}
			catch (...)
			{
				qDebug()<<"EXCEPTION,RT label connect to a symbol without imName";
				std::cout<<(worldModel->getSymbol( edge->getSymbolPair().second))->toString(true);
			}
		}
}
