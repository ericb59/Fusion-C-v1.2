#include <tile.h>
#include<QDebug>

// Constuctor y destructor:
CTile::CTile() {
    Reset();
}

void CTile::Reset() {
    int x, y;
    for( y = 0; y < 8; y++ )  {
        m_BgColors[y]   = 0; // 1;
        m_ForeColors[y] = 1;
        for( x = 0; x < 8; x++ ) {
            m_Pixels[x][y] = 0;
        }
    }
}

// Getters:
int CTile::GetForeColor( int y ) {
    y &= 7;
    return m_ForeColors[y];
}

int CTile::GetBgColor( int y ) {
    y &= 7;
    return m_BgColors[y];
}

int CTile::GetPixel( int x, int y ) {
    x &= 7; y &= 7;
	return m_Pixels[x][y];
}

int CTile::GetPixelColor( int x, int y ) {
    x &= 7; y &= 7;
    if( m_Pixels[x][y] == 1 ) return m_ForeColors[y];
    return m_BgColors[y];
}

int CTile::GetRowPattern( int row ) {
    row &= 7;
    int value;
    value = 0;
    for( int i = 7; i >=0; i-- ) {
        value = value | ( m_Pixels[7-i][row] << i );
    }
    return value;
}

int CTile::GetRowColor( int row ) {
    row &= 7;
    return ( m_ForeColors[row] << 4 ) | m_BgColors[row];
}

// Setters:
void CTile::SetForeColor( int y, int color ) {
    y &= 7;
    color &= 15;
    m_ForeColors[y] = color;
}

void CTile::SetBackColor( int y, int color ) {
    y &= 7;
    color &= 15;
    m_BgColors[y] = color;
}

void CTile::SetPixel( int x, int y, int color ) {
    x &= 7; y &= 7;
    color &= 1;
	m_Pixels[x][y] = color;
}

void CTile::SetForePixel( int x, int y ) {
    x &= 7; y &= 7;
    m_Pixels[x][y] = 1;
}

void CTile::SetBackPixel( int x, int y ) {
    x &= 7; y &= 7;
    m_Pixels[x][y] = 0;
}

void CTile::SetRowPattern( int row, quint8 value ) {
    row &= 7;
    for( int i = 7; i >= 0; i-- ) {
        m_Pixels[7-i][row] = ( value >> i ) & 0x01;
    }
}

void CTile::SetRowColor( int row, quint8 color ) {
    row &= 7;
    m_ForeColors[row] = (color & 0xF0) >> 4;
    m_BgColors[row]   = color & 0x0F;
}

// Others:
bool CTile::IsBlack() {
    for( int i = 0; i < 8; i++ ) {
		if( !( m_BgColors[i] <= 1 && m_ForeColors[i] <= 1 ) ) return false;
	}
	return true;
}

void CTile::ReorderLessFirst() {
    for( unsigned int y = 0; y < 8; y++ ) {
        if( m_BgColors[y] > m_ForeColors[y] ) //Then exchange colors
        {
            unsigned int tmp = m_BgColors[y];
            m_BgColors[y] = m_ForeColors[y];
            m_ForeColors[y] = tmp;
            for( unsigned int x = 0; x < 8; x++ ) {
                m_Pixels[x][y] = !m_Pixels[x][y];
            }
        }
    }
}

void CTile::Reorder() {
	int tmp;
	int y, x;
	int most;
	int max;
	int colors[16];
	for( x = 0; x < 16; x++ ) colors[x] = 0;
    for( y = 0; y < 8; y++ ) {
		for( x = 0, tmp = 0; x < 8; x++ ) if( m_Pixels[x][y] == 1 ) tmp++;
		colors[ m_ForeColors[y] ] += tmp;
		colors[ m_BgColors[y] ] += ( 8 - tmp );
	}
    for( y = 0, most = 0, max = 0; y < 16; y++ ) {
        if( colors[y] > max ) {
			max = colors[y];
			most = y;
		}
	}
    for( y = 0; y < 8; y++ ) {
		if( m_ForeColors[y] == most ) InvertRow( y );	
	}
}

void CTile::ShiftUp() {
	int y, x;
    for( y = 0; y < 7; y++ ) {
		for( x = 0; x < 8; x++ ) m_Pixels[x][y] = m_Pixels[x][y+1];
		m_ForeColors[y] = m_ForeColors[y+1];
		m_BgColors[y] = m_BgColors[y+1];
	}
	for( x = 0; x < 8; x++ ) m_Pixels[x][y] = 0;
	m_ForeColors[y] = 0;
	m_BgColors[y] = 0;
}

void CTile::ShiftDown() {
	int y, x;
    for( y = 7; y > 0; y-- ) {
		for( x = 0; x < 8; x++ ) m_Pixels[x][y] = m_Pixels[x][y-1];
		m_ForeColors[y] = m_ForeColors[y-1];
		m_BgColors[y] = m_BgColors[y-1];
	}
	for( x = 0; x < 8; x++ ) m_Pixels[x][y] = 0;
	m_ForeColors[y] = 0;
	m_BgColors[y] = 0;
}

void CTile::ShiftRight() {
	int y, x;
    for( y = 0; y < 8; y++ ) {
		for( x = 7; x > 0; x-- ) m_Pixels[x][y] = m_Pixels[x-1][y];		
		m_Pixels[x][y] = 0;
	}
}

void CTile::ShiftLeft() {
	int y, x;
    for( y = 0; y < 8; y++ ) {
		for( x = 0; x < 7; x++ ) m_Pixels[x][y] = m_Pixels[x+1][y];		
		m_Pixels[x][y] = 0;
	}
}

void CTile::FlipHorizontal() {
	int tmp;
	int x, y, j;
    for( y = 0; y < 8; y++ ) {
        for( x = 0, j = 7; x < 4; x++, j-- ) {
			tmp = m_Pixels[x][y];
			m_Pixels[x][y] = m_Pixels[j][y];
			m_Pixels[j][y] = tmp;
		}
	}
}

void CTile::FlipVertical() {
	int tmp;
	int x, y, j;
    for( y = 0, j = 7; y < 4; y++, j-- ) {
        for( x = 0; x < 8; x++ ) {
			tmp = m_Pixels[x][y];
			m_Pixels[x][y] = m_Pixels[x][j];
			m_Pixels[x][j] = tmp;			
		}
		tmp = m_ForeColors[y];
		m_ForeColors[y] = m_ForeColors[j];
		m_ForeColors[j] = tmp;		
		tmp = m_BgColors[y];
		m_BgColors[y] = m_BgColors[j];
		m_BgColors[j] = tmp;
	}
}

void CTile::Rotate() {
	int x, y;
	int tmp[8][8];
    for( y = 0; y < 8; y++ )  {
		for( x = 0; x < 8; x++ ) tmp[x][y] = m_Pixels[x][y];
	}
    for( y = 0; y < 8; y++ ) {
		for( x = 0; x < 8; x++ ) m_Pixels[y][7-x] = tmp[x][y];
	}
}

void CTile::InvertRow( int y ) {
    y &= 7;
	int x;
	int tmp;
    for( x = 0; x < 8; x++ ) m_Pixels[x][y] = !m_Pixels[x][y];
	tmp = m_BgColors[y];
	m_BgColors[y] = m_ForeColors[y];
	m_ForeColors[y] = tmp;
}

void CTile::ChangeColors( QList<QPoint> changes ) {
    QPoint p;
    for( int i = 0; i < 8; i++ ) {
        for( int j = 0; j < changes.size(); j++ ) {
            p = changes.at( j );
            if( m_ForeColors[i] == p.x() ) {
                m_ForeColors[i] = p.y();
                break;
            }
        }
        for( int j = 0; j< changes.size(); j++ ) {
            p = changes.at( j );
            if( m_BgColors[i] == p.x() ) {
                m_BgColors[i] = p.y();
                break;
            }
        }
    }
}

CTile& CTile::operator = ( const CTile& copy ) {
    for( int i = 0; i < 8; i++ ) {
        m_ForeColors[i] = copy.m_ForeColors[i];
        m_BgColors[i]   = copy.m_BgColors[i];
        for( int j = 0; j < 8; j++ ) {
            m_Pixels[i][j] = copy.m_Pixels[i][j];
        }
    }
    return *this;
}

