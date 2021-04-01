// function for splitting image into multiple blocks. rowDivisor and colDivisor specify the number of blocks in rows and cols respectively
        int subdivide(const Mat &img, const int rowDivisor, const int colDivisor, vector<Mat> &blocks)
        {
            /* Checking if the image was passed correctly */
            if(!img.data || img.empty())
                cerr << "Problem Loading Image" << endl;

            /* Cloning the image to another for visualization later, if you do not want to visualize the result just comment every line related to visualization */
            Mat maskImg = img.clone();
            /* Checking if the clone image was cloned correctly */
            if(!maskImg.data || maskImg.empty())
                cerr << "Problem Loading Image" << endl;

            // check if divisors fit to image dimensions
            if(img.cols % colDivisor == 0 && img.rows % rowDivisor == 0)
            {
                for(int y = 0; y < img.cols; y += img.cols / colDivisor)
                {
                    for(int x = 0; x < img.rows; x += img.rows / rowDivisor)
                    {
                        blocks.push_back(img(cv::Rect(y, x, (img.cols / colDivisor), (img.rows / rowDivisor))).clone());
                        rectangle(maskImg, Point(y, x), Point(y + (maskImg.cols / colDivisor) - 1, x + (maskImg.rows / rowDivisor) - 1), CV_RGB(255, 0, 0), 1); // visualization

                        imshow("Image", maskImg); // visualization
                        waitKey(0); // visualization
                    }
                }
            }else if(img.cols % colDivisor != 0)
            {
                cerr << "Error: Please use another divisor for the column split." << endl;
                exit(1);
            }else if(img.rows % rowDivisor != 0)
            {
                cerr << "Error: Please use another divisor for the row split." << endl;
                exit(1);
            }
        return EXIT_SUCCESS;
    }

//picked from https://answers.opencv.org/question/53694/divide-an-image-into-lower-regions/
