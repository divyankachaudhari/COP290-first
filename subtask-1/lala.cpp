#include<iostream>
#include<regex>
using namespace std;

bool isImageFile(string str) {
  // Regex to check valid image file extension.
  const regex pattern("[^\\s]+(.*?)\\.(jpg|jpeg|png|gif|JPG|JPEG|PNG|GIF)$");
  if (str.empty()){ return false; }
  if(regex_match(str, pattern)){ return true;}
  else { return false;}
}

int main(){
string str;
cout<<"Enter string you fucker ";
cin>>str;
if(isImageFile(str)== false){
cout<<"fucker, that's not an image oh god";
}
cout<< "nice, a strrrinnng";
return 0;

}
